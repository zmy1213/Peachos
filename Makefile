FILES = ./build/kernel.asm.o
all:./bin/boot.bin $(FILES)
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin 

./bin/kernel.bin :$(FILES)
	arm-none-eabi-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	arm-none-eabi-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -o0 -nostdlib ./build/kernelfull.o
./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o
clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/os.bin
	rm -rf ./build/kernel.asm.o