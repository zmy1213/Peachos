#bin/bash
export PREFIX="$HOME/zmy/x86arm/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi"
export TARGET=arm-none-eabi
export PATH="$PREFIX/bin/:$PATH"
make all