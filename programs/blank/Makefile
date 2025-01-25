FILES=./build/blank.o
INCLUDES= -I../stdlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
all:
	nasm -f elf ./blank.asm -o ./build/blank.o
	i686-elf-gcc -g -T ./linker.ld -o ./blank.bin -ffreestanding -O0 -nostdlib -fpic -g  ./build/blank.o

./build/blank.o: ./blank.c
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./blank.c -o ./build/blank.o

clean:
	rm -rf ./build/blank.o