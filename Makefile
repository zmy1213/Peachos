FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/fs/pparser.o ./build/fs/file.o ./build/fs/fat16/fat16.o ./build/stringos/stringos.o ./build/disk/streamer.o
INCLUDES = -I./src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
all:./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin 
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	
#mount file system
	hdiutil attach -imagekey diskimage-class=CRawDiskImage -nomount ./bin/os.bin 
	sudo mount -t msdos /dev/disk4 /mnt/d   
	cp /Users/zhuzhumingyang/Desktop/test1.txt /mnt/d
	hdiutil detach /dev/disk4 

./bin/kernel.bin :$(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o
./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o
./build/kernel.o: ./src/kernel.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) $(FLAGS) -c ./src/kernel.c -o ./build/kernel.o 
./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/idt $(FLAGS) -c ./src/idt/idt.c -o ./build/idt/idt.o 
./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/memory $(FLAGS) -c ./src/memory/memory.c -o ./build/memory/memory.o 
./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o
./build/memory/heap/heap.o:./src/memory/heap/heap.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/memory/heap $(FLAGS) -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o 
./build/memory/heap/kheap.o:./src/memory/heap/heap.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/memory/heap $(FLAGS) -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o 
./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/memory/paging $(FLAGS) -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o 

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/disk $(FLAGS) -c ./src/disk/disk.c -o ./build/disk/disk.o 

./build/fs/pparser.o: ./src/fs/pparser.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/fs $(FLAGS) -c ./src/fs/pparser.c -o ./build/fs/pparser.o 

./build/stringos/stringos.o: ./src/stringos/stringos.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/stringos $(FLAGS) -c ./src/stringos/stringos.c -o ./build/stringos/stringos.o 
./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/disk $(FLAGS) -c ./src/disk/streamer.c -o ./build/disk/streamer.o 

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/fs $(FLAGS) -c ./src/fs/file.c -o ./build/fs/file.o 

./build/fs/fat16/fat16.o: ./src/fs/fat16/fat16.c | build/fs/fat16/
	i686-elf-gcc -std=gnu99 $(INCLUDES) -I./src/fs $(FLAGS) -c ./src/fs/fat16/fat16.c -o ./build/fs/fat16/fat16.o 
build/fs/fat16/:
	mkdir -p ./build/fs/fat16
clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o