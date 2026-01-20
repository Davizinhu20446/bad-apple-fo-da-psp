cmake_minimum_required(VERSION 3.11)
project(BadAppleFoDaPSP)

# Se o seu compilador reclamar de include, adicione:
# include_directories(${PSPDEV}/psp/sdk/include)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspaudiolib
    pspaudio
    pspdebug
    pspdisplay
    pspge
    pspctrl
    psputility
    pspuser
)

# Em vez de include(PspSdk), usamos a função direta se ela estiver disponível no seu toolchain
# Caso contrário, o comando abaixo basta para gerar o PBP
create_pbp_file(
    TARGET ${PROJECT_NAME}
    TITLE "Bad Apple Fo Da PSP"
    VERSION 01.00
)