@echo off
REM 1. create freedos.img by 
REM    qemu-img create -f raw imag_file 100M
REM 2. install freedos.iso by
REM    qemu -hda freedos.img -cdrom fdbootcd.iso -boot d
REM 3. use folder as drive
REM

set path=%path%;d:\qemu

qemu-system-i386 -device ahci,id=ahci0 -hda freedos.img -hdc fat:foldhdd