对于操作系统的学习

BIOS对应的中文术语是“基本输入输出系统”。计算机启动时，首先运行的便是BIOS。

BIOS是计算机厂商预置在计算机硬件中的一种软件，它会完成一些操作。我们只需知道，它会从内存地址0x7c00处读取引导扇区，就足够了。引导扇区的作用是从软盘中读取加载器。

我们把”引导扇区“叫做boot，把引导扇区的源码文件命名为boot.asm。boot恰好占用一个扇区，因此，boot所在的扇区被称为“引导扇区”。我们就把“加载器”叫做loader吧，把加载器的源码文件命名为loader.asm。

我们将使用nasm编写boot和loader。


什么是BIOS中断？我们不必纠缠这个概念，先从我们熟悉的高级语言的角度理解int 13h。

把int 13h理解成一个函数，把这个函数命名为ReadSectorFromFloppy。


times	510 - ($ - $$)	db	0
dw	0xAA55
当前指令的前510个补0，然后加上magic数（引导扇区 label）

nasm -f bin ./boot.asm -o ./boot.bin 编译
ndisasm boot.bin反汇编


qemu-system-x86_64 -hda ./boot.bin 输出字符串

db字节
SI源变址寄存器，DI目地变址寄存器， 都是变址寄存器，都是在某个地址的基础上进行偏移变化，因此都需要基址寄存器。
SI DI 一般与数据段寄存器DS联用，用来确定数据段中某一存储单元的地址。这两个寄存器有自动增量和自动减量的功能所以用于变址是很方便的


movsb、movsw、movsd	传送字符串数据：将esi寻址的内存数据复制到edi寻址的内存位置, 常和重复指令配合使用
cmpsb、cmpsw、cmpsd	比较字符串：比较分别由esi和edi寻址的内存数据
scasb、scasw、scasd	扫描字符串：比较累加器（AL、AX、EAX）与EDI寻址的内存数据
stosb、stosw、stosd	保存字符串数据：将累加器（AL、AX、EAX）内容保存到edi寻址的内存位置
lodsb、lodsw、lodsd	将字符串加载到累加器：将DS:ESI寻址的内存数据加载到累加器

JE	Jump if left Equals right

db 'Hello World!',0 0代表着\0

real mode:
1 mb of ram accessible 
2 based of original x86
3 no security 
4 16 bits accrssible at one time

段寄存器：cs，ss，ds，es
 
cli sti 是改变段地址时常用的操作，防止中断时带来的错误

注意段寄存器不能直接寻址

NOP"指令即空指令
运行该指令时单片机什么都不做，但是会占用一个指令的时间。

jmp short 这种格式的jmp指令实现的是上述的段内短转移，修改范围为-128～127

是指中断服务程序入口地址的偏移量与段基值，一个中断向量占据4字节空间。中断向量表是8086系统内存中最低端1K字节空间，它的作用就是按照中断类型号从小到大的顺序存储对应的中断向量，总共存储256个中断向量。在中断响应过程中，CPU通过从接口电路获取的中断类型号（中断向量号）计算对应中断向量在表中的位置，并从中断向量表中获取中断向量，将程序流程转向中断服务程序的入口地址。
四个字节 offset + segment

int指令是X86汇编语言中最重要的指令之一。它的作用是引发中断
执行“int n”时，CPU从中断向量表中，找到第n号表项，修改CS和IP
int指令执行过程
1）取中断类型码n；
2）标志寄存器入栈（pushf），IF=0，TF=0（重置中断标志位）；
3）CS、IP入栈；
4）查中断向量表， (IP)=(n*4)，(CS)=(n*4+2)。

文件并不存在 
数据是写在扇区（512）的
reading the sector of the disk will return 512 bytes for the chosen sector

①CHS寻址模式将硬盘划分为磁头（Heads）、柱面(Cylinder)、扇区(Sector)。

△磁头(Heads)：每张磁片的正反两面各有一个磁头，一个磁头对应一张磁片的一个面。因此，用第几磁头就可以表示数据在哪个磁面。

△柱面(Cylinder)：所有磁片中半径相同的同心磁道构成“柱面"，意思是这一系列的磁道垂直叠在一起，就形成一个柱面的形状。简单地理解，柱面数=磁道数。

△扇区(Sector)：将磁道划分为若干个小的区段，就是扇区。虽然很小，但实际是一个扇子的形状，故称为扇区。每个扇区的容量为512字节。

②知道了磁头数、柱面数、扇区数，就可以很容易地确定数据保存在硬盘的哪个位置。也很容易确定硬盘的容量，其计算公式是：硬盘容量＝磁头数×柱面数×扇区数×512字节

lba:计算，
sector = num / 512
offset = num % 512

16 real mode the bios provide interrupt for disk operations
32 system you have to create disk driver which is little more complicated 

dd 可从标准输入或文件中读取数据，根据指定的格式来转换数据，再输出到文件、设备或标准输出。

AH = 03h
AL = number of sectors to write (must be nonzero)
CH = low eight bits of cylinder number
CL = sector number 1-63 (bits 0-5)
high two bits of cylinder (bits 6-7, hard disk only)
DH = head number
DL = drive number (bit 7 set for hard disk).//自动设置
ES:BX -> data buffer

Return:
CF set on error
CF clear if successful
AH = status (see #00234)
AL = number of sectors transferred
(only valid if CF set for some BIOSes)

实模式与保护模式的区别是什么
实模式 16 位，保护模式 32 位
实模式下的地址是段寄存器地址偏移4位+偏移地址得到物理地址。保护模式下段寄存器存入了段选择子，在段描述符表中寻找段基址，再加上偏移地址得到物理地址（开启分页下为逻辑地址）
这个我觉得是个 1 的推论，就是实模式寻址空间是 1M，保护模式是 4G
这个我觉得是 2 的推论，就是段描述符表记录了段的权限，改变了实模式下可以随意访问所有内存的隐患（这也是保护这两个字的体现）

cli            ; disable interrupts
lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
mov eax, cr0 
or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
mov cr0, eax
 
; Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor) 
; to load CS with proper PM32 descriptor)
jmp 08h:PModeMain

段选择子。第2位存储了TI值（0代表GDT，1代表LDT），第0、1位存储了当前的特权级（CPL）。
详情请见图片

例如在保护模式下执行汇编代码mov ds:[si], ax的大致步骤如下：

首先CPU需要查找GDT在内存中位置，GDT的位置从GDTR寄存器中直接获取
然后根据DS寄存器得到目标段描述符的物理地址
计算出描述符中的段基址的值加上SI寄存器存储的偏移量的结果，该结果为目标物理地址
将AX寄存器中的数据写入到目标物理地址

CPU切换到保护模式前，需要准备好GDT数据结构，并执行LGDT指令，将GDT基地址和界限传入到GDTR寄存器。

GDTR寄存器长度为6字节（48位），前两个字节为GDT界限，后4个字节为GDT表的基地址。所以说，GDT最多只能拥有8192个描述符（65536 / 8）。

段基址（32位），占据描述符的第16～39位和第55位～63位，前者存储低16位，后者存储高16位
段界限（20位），占据描述符的第0～15位和第48～51位，前者存储低16位，后者存储高4位。
段属性（12位），占据描述符的第39～47位和第49～55位，段属性可以细分为8种：TYPE属性、S属性、DPL属性、P属性、AVL属性、L属性、D/B属性和G属性。

其中的段属性可以寻找资料查找



gdb测试

当ibm pc at系统被制造出来时，新的286处理器以及以后版本和旧有x86处理器并不兼容。旧的X86处理器（Intel 8086）有20位地址总线，这样可以访问最高1M内存。而386和以后版本有32地址总线，这样可以访问最高4G的内存。但是旧的8086处理器没有这么大的地址总线。为了保持兼容性Intel在地址线的第20位上制造了一个逻辑OR门，以便可以开启或关闭超过20位的地址总线。这样，为了兼容旧的处理器，在机器开启时A20被禁止的。
Linker scipt https://www.cnblogs.com/jianhua1992/p/16852784.html

setting the entry point
entroy(symbol) //设置入口点

fileoperator

include input(file file)
output(filename) 

*(.text) ’ 表示所有输入文件中的所有 ‘.text*’ 输入段。

剩下的行定义了定义输出文件中的‘.data ’ 和‘.bss ’ 段。链接器会将‘.data ’ 输出段放置在地址’0x8000000 ’处。在链接器放置‘.data ’ 段后，位置计数器为’0x8000000 ’加上‘.data ’ 段的大小。因此‘.bss ’ 输出段在内存中将会紧紧挨在‘.data ’段后面。
COMMON 指令用于包含在多个对象文件之间共享的未初始化数据。

nasm -f elf -g ./src/kernel.asm ./build/kernel.asm.o
-f 参数指定输出格式为 ELF 格式。-g 参数表示生成调试信息，这将在编译时将调试信息嵌入目标文件中。

ELF（Executable and Linkable Format）是一种用于表示可执行文件、共享库和目标文件的文件格式。它是现代Unix和Linux系统上使用的主要可执行文件格式之一。
ELF 文件格式是由各种头部、节（section）和段（segment）组成的。头部包含文件类型、目标机器类型、入口点地址等元数据信息。节是文件中一个逻辑部分的单元，例如代码、数据、符号表、重定位表等。段是指将一组节打包成可加载的逻辑单元，例如可执行代码段、数据段、只读段等。

arm-none-eabi-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
这段代码使用 arm-none-eabi-ld 链接器将多个目标文件 $(FILES) 链接成一个可重定位目标文件 ./build/kernelfull.o。以下是代码的解释：
arm-none-eabi-ld 是一种 ARM 架构专用的链接器，用于将目标文件（如汇编器生成的 .o 文件）合并成一个可执行程序或共享库。
-g 参数表示将调试信息嵌入到可执行文件中，以便在程序崩溃或出现错误时进行调试。
-relocatable 参数表示将生成一个可重定位的目标文件，这种目标文件包含位置无关的代码和数据，可以在内存的任何位置加载。
$(FILES) 是包含要链接的目标文件路径的变量。
-o 参数指定输出文件的路径和名称。
因此，该命令将多个目标文件链接到一起，并生成一个可重定位目标文件 ./build/kernelfull.o，其中包含了调试信息。可重定位目标文件通常用于构建可执行文件或共享库的过程中，作为中间产物。

arm-none-eabi-gcc -T ./linker.ld -o ./bin/kernel.bin -ffreestanding -o0
这段代码使用 arm-none-eabi-gcc 编译器将 ./linker.ld 链接脚本文件指定的目标文件链接成一个自由立足（freestanding）的、无操作系统支持的可执行二进制文件 ./bin/kernel.bin。以下是代码的解释：

arm-none-eabi-gcc 是一种 ARM 架构专用的编译器，用于将 C/C++ 代码编译成可执行文件。
-T 参数指定链接脚本文件的路径。链接脚本文件用于描述可执行文件的内存布局和各个部分之间的关系。
-o 参数指定输出文件的路径和名称。
-ffreestanding 参数表示编译器应该生成一个无操作系统支持的可执行文件。这意味着程序需要自己处理硬件和系统资源，而不能依赖于操作系统提供的支持。
-O0 参数表示关闭优化，以便在调试时更容易理解生成的代码。
因此，该命令将链接脚本文件指定的目标文件链接成一个自由立足的、无操作系统支持的可执行二进制文件 ./bin/kernel.bin，该可执行文件可以被加载到内存中执行。

.global 是为了让一个符号对编译器可见
dd if=./dev/zero >> ./bin/os.bin
/dev/zero 是一个特殊的设备文件，它会返回一个连续的 0 字节流。在这里，它用作输入文件以向 os.bin 写入连续的 0 字节，这通常用于填充文件以达到特定的大小。
综上，这个指令的作用是将 ./bin/os.bin 文件的大小增加至指定大小，而其中的内容都为 0。

; 将LBA地址存储到eax寄存器中
mov eax, lba       

; 将eax寄存器的高8位（即第24-31位）存储到ebx寄存器中
mov ebx, eax
shr eax, 24

; 向0x1F6端口发送命令
mov dx, 0x1F6
out dx, al

; 将扇区数存储到eax寄存器中
mov eax, sector_count

; 向0x1F2端口发送命令
mov dx, 0x1F2
out dx, al

; 将LBA地址的第0-7位存储到0x1F3端口
mov eax, ebx
mov dx, 0x1F3
out dx, al

; 将LBA地址的第8-15位存储到0x1F4端口
mov eax, ebx
shr eax, 8
mov dx, 0x1F4
out dx, al

; 将LBA地址的第16-23位存储到0x1F5端口
mov eax, ebx
shr eax, 16
mov dx, 0x1F5
out dx, al

; 向0x1F7端口发送命令，读取硬盘数据
mov edx, 0x1F7
mov al, 0x20
out dx, al