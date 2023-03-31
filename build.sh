#bin/bash
export PREFIX="/usr/local/crossgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin/:$PATH"
make all