#bin/bash
export PREFIX="/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all
