#include <pspuser.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

PSP_MODULE_INFO("BadAppleFoDaPSP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(-256);

#define FPS 30
#define FRAME_TIME_US (1000000 / FPS)
#define ROWS 30
#define COLS 62
#define FRAME_SIZE 1920 // Valor exato baseado no seu arquivo .bin (12.618.240 / 6572)
#define FRAME_COUNT 6572

static char (*all_frames)[FRAME_SIZE] = NULL;
static volatile int audio_playing = 0;
static volatile int audio_enabled = 1;

int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

void audio_callback(void *buf, unsigned int reqn, void *pdata) {
    static int audio_file = -1;
    static int header_skipped = 0;
    
    if (!audio_playing || !audio_enabled) {
        if (buf) memset(buf, 0, reqn * 4);
        return;
    }
    
    if (audio_file < 0) {
        // Tentativa de abrir com caminho relativo explícito para evitar erro de diretório
        audio_file = sceIoOpen("ms0:PSP/GAME/BadAppleFoDaPSP/audio.wav", PSP_O_RDONLY, 0777);
        if (audio_file < 0) audio_file = sceIoOpen("./badapple.wav", PSP_O_RDONLY, 0777);
        
        if (audio_file < 0) {
            audio_playing = 0;
            if (buf) memset(buf, 0, reqn * 4);
            return;
        }
    }
    
    if (!header_skipped) {
        sceIoLseek(audio_file, 44, PSP_SEEK_SET);
        header_skipped = 1;
    }
    
    int bytes_to_read = reqn * 4; // reqn * 2 canais * 2 bytes (16-bit)
    int read = sceIoRead(audio_file, buf, bytes_to_read);
    
    if (read <= 0) {
        sceIoLseek(audio_file, 44, PSP_SEEK_SET); // Loop
        if (buf) memset(buf, 0, reqn * 4);
    } else if (read < bytes_to_read) {
        memset((char*)buf + read, 0, bytes_to_read - read);
    }
}

int load_all_frames_bin(void) {
    pspDebugScreenSetXY(0, 2);
    
    // Abre o arquivo explicitando o diretório atual
    int fd = sceIoOpen("ms0:/PSP/GAME/BadAppleFoDaPSP/frames.bin", PSP_O_RDONLY, 0777);
    if (fd < 0) {
        pspDebugScreenPrintf("ERROR: frames.bin not found! (Code: %d)\n", fd);
        return 0;
    }

    pspDebugScreenPrintf("Allocating RAM...\n");
    all_frames = (char (*)[FRAME_SIZE])malloc(FRAME_COUNT * FRAME_SIZE);
    if (!all_frames) {
        sceIoClose(fd);
        return 0;
    }
    
    pspDebugScreenPrintf("Loading frames.bin to RAM...\n");
    int total_read = sceIoRead(fd, all_frames, FRAME_COUNT * FRAME_SIZE);
    sceIoClose(fd);

    return (total_read > 0);
}

void draw_frame_from_memory(int index) {
    if (index < 1 || index > FRAME_COUNT || !all_frames) return;
    pspDebugScreenSetXY(0, 0);

    char *frame_data = all_frames[index - 1];
    int pos = 0;

    for (int row = 0; row < ROWS; row++) {
        char line[COLS + 1];
        int line_pos = 0;
        while (pos < FRAME_SIZE) {
            char ch = frame_data[pos++];
            if (ch == '\r') continue;
            if (ch == '\n') break;
            if (line_pos < COLS) line[line_pos++] = ch;
        }
        while (line_pos < COLS) line[line_pos++] = ' ';
        line[COLS] = '\0';
        pspDebugScreenPrintf("%s\n", line);
    }
}

int main(void) {
    setup_callbacks();
    pspDebugScreenInit();
    
    pspDebugScreenPrintf("Bad Apple ASCII Animation - PSP\n");
    
    if (!load_all_frames_bin()) {
        pspDebugScreenPrintf("Press START to exit.\n");
        while(1) {
            SceCtrlData pad;
            sceCtrlReadBufferPositive(&pad, 1);
            if (pad.Buttons & PSP_CTRL_START) sceKernelExitGame();
        }
    }
    
    // Inicialização robusta de áudio
    pspAudioInit();
    // Reserva o canal 0 e define volume máximo imediatamente
    sceAudioChReserve(0, 512, PSP_AUDIO_FORMAT_STEREO);
    pspAudioSetVolume(0, 0x8000, 0x8000); 
    pspAudioSetChannelCallback(0, audio_callback, NULL);
    
    pspDebugScreenPrintf("Ready! Starting in 2s...\n");
    sceKernelDelayThread(2000000);
    
    audio_playing = 1;
    SceCtrlData pad, oldpad;
    memset(&oldpad, 0, sizeof(oldpad));
    u64 animation_start = sceKernelGetSystemTimeWide();
    
    for (int frame = 1; frame <= FRAME_COUNT; frame++) {
        u64 target_time = animation_start + (u64)(frame - 1) * FRAME_TIME_US;
        
        // Espera o tempo exato do frame permitindo que outras threads (áudio) trabalhem
        while (sceKernelGetSystemTimeWide() < target_time) {
            sceKernelDelayThread(100); 
        }
        
        draw_frame_from_memory(frame);
        
        sceCtrlReadBufferPositive(&pad, 1);
        if ((pad.Buttons & PSP_CTRL_SQUARE) && !(oldpad.Buttons & PSP_CTRL_SQUARE)) {
            audio_enabled = !audio_enabled;
            pspAudioSetVolume(0, audio_enabled ? 0x8000 : 0, audio_enabled ? 0x8000 : 0);
        }
        if (pad.Buttons & PSP_CTRL_START) break;
        oldpad = pad;
        
        sceDisplayWaitVblankStart();
    }
    
    audio_playing = 0;
    sceKernelDelayThread(100000);
    if (all_frames) free(all_frames);
    sceKernelExitGame();
    return 0;
}