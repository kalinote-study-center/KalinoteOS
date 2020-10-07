@set SDL_VIDEODRIVER=windib
@set QEMU_AUDIO_DRV=none
@set QEMU_AUDIO_LOG_TO_MONITOR=0
qemu.exe -L . -m 1024 -localtime -std-vga -fda fdimage0.bin