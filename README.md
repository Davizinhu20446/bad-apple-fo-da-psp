
# Bad Apple!! on the PSP (ASCII)

> *PSP fo da PSP.*

Bad Apple!! running on a **real PlayStation Portable**, rendered entirely using **ASCII frames** with `pspDebugScreen`, synced with audio playback.

No GU.  
No GPU.  
No fancy tricks.

Just **pure PSP homebrew suffering**.

---

## ⚠️ If you are going to manually build it

Please note that the file ```audio.mp3``` in the ```/sources/``` folder is in a format that the PSP doesn't like on purporse. Since the wave file itself is too _thicc_ to get into the repo, you gotta manually convert the mp3 into wav. Get your hands on ffmpeg and run this command on Windows PowerShell:

```ffmpeg -i audio.mp3 -acodec pcm_s16le -ar 44100 -ac 2 audio.wav```

Then you are good to go!

## 🎥 What is this?

This project plays **Bad Apple!!** on the PSP by:

- Rendering pre-generated **ASCII frames**
- Playing synced **16-bit stereo audio**
- Using `pspDebugScreen` for rendering
- Loading a large binary frame file directly into RAM

This is **not a video**.  
This is **not emulation**.  
This is **not GU-based rendering**.

It is a dumb idea executed successfully.

---

### Release version includes ONLY:
- `EBOOT.PBP`
- `audio.wav`
- `frames.bin`

---

## 🧠 How it works

- All ASCII frames are preprocessed and packed into a single `frames.bin` (~12 MB)
- Frames are loaded into RAM
- Each frame is rendered using `pspDebugScreen`
- Audio is streamed from `audio.wav`
- Frame timing is locked to audio playback

---

## ⚠️ Compatibility Table

| Console | Compatible | Reason |
|--------|------------|--------|
| PSP 1000 | ⚠️ | Untested. Limited RAM (32 MB). `frames.bin` may cause RAM overflow |
| PSP 2000 | ✅ | Untested, but surely working |
| PSP 3000 | ✅ | Tested and working |
| PSP Go | ⚠️ | Uses `ms0:` by default. Requires source rebuild for `ef0:` |
| PSP Street (E1000) | ❓ | Not tested |
| Emulator (PPSSPP) | ❌ | Crashes upon launch. Don't even try

---

## 📌 Important Notes

- Default file paths use `ms0:`  
- PSP Go users **must rebuild from source** using `ef0:`  
- This project **will NOT work** reliably on PSP 1000 due to RAM constraints  
- Debug rendering means **performance is limited but stable**

---

## 🛠 Building from Source

You **must** have a working PSPSDK.

> This project does **NOT** include the SDK. Follow the tutorial linked down below

### Run this stuff on the WSL terminal inside VS Code:

```bash
cd build
psp-cmake ..
make
```

If successful, you will get:

EBOOT.PBP


---

🟢 PSP Go Users (IMPORTANT)

PSP Go uses ef0: instead of ms0:.

Steps:

1. Clone the repository (do NOT use the Release)


2. Set up PSPSDK (follow the tutorial linked below)


3. Open main.c


4. Replace every occurrence of:
```C
ms0:
```

with:
```C
ef0:
```

5. Build the project:
```
cd build
psp-cmake ..
make
```

6. Copy files to:

ef0:/PSP/GAME/BadAppleOnDaPSP/


7. Run from XMB




---

📚 PSPSDK Setup

This project assumes you already followed a PSPSDK tutorial.

If you just wanna the EBOOT.PBP file, just go to the [Releases page](https://github.com/Davizinhu20446/bad-apple-fo-da-psp/releases).

If you are a PSP Go user / wants to mod the app, follow [This tutorial](https://pspdev.github.io/installation.html) (Installlation and How to use) if you don't have PSPSDK

⚠️ Do NOT download random SDK builds without understanding them. You will probably get a trojan, or malware...

⚠️ Do NOT mod this without crediting me, read the LICENSE file to learn more.


---

📜 Legal / Copyright

Bad Apple!! has been remixed and reimplemented countless times

This project contains no original video

ASCII frames are generated data

Audio is user-provided in source builds


If this repository ever gets DMCA’d:

> Skill issue.




---

🙏 Credits

Original Bad Apple!! creators

PSP homebrew community

Everyone who suffered with PSPSDK

Me, for not giving up



---

💀 Final Words

Could this have been done in one day?

ABSOLUTELY (psp-gcc sucks).

Was it?

No.

Would I do it again?

Also no.

Do I want to make the GCC cease from this world?

**HECK YEAH**
