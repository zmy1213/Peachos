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

中断向量是指中断服务程序入口地址的偏移量与段基值，一个中断向量占据4字节空间。中断向量表是8086系统内存中最低端1K字节空间，它的作用就是按照中断类型号从小到大的顺序存储对应的中断向量，总共存储256个中断向量。在中断响应过程中，CPU通过从接口电路获取的中断类型号（中断向量号）计算对应中断向量在表中的位置，并从中断向量表中获取中断向量，将程序流程转向中断服务程序的入口地址。
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

 mov ebx,eax
    or eax,0xE0 默认补0
    shr eax,24
    mov dx,0x1F6
    out dx,al;0x01 lba模式 flag

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

在load 32标签下，将1存储在eax寄存器中，将100存储在ecx寄存器中，将0x0100000存储在edi寄存器中。

调用ata_lba_read标签，并将返回值存储在eax寄存器中。

在ata_lba_read标签下，将eax寄存器的值右移24位（相当于取出eax寄存器的高8位），然后将0x1F6存储在dx寄存器中。接着，将dx和al寄存器的值通过out指令输出到磁盘控制器的端口，用于发送LBA地址和选择磁盘驱动器。

将ecx寄存器的值通过mov指令复制到eax寄存器中，然后将0x1F2存储在dx寄存器中。接着，将dx和al寄存器的值通过out指令输出到磁盘控制器的端口，用于设置要读取的扇区数。

将eax寄存器的值（ata_lba_read返回的结果）通过mov指令复制到ebx寄存器中，然后将0x1F3存储在dx寄存器中。接着，将dx和al寄存器的值通过out指令输出到磁盘控制器的端口，用于设置要读取的扇区的起始LBA地址的低字节。

将0x1F4存储在dx寄存器中，然后将ebx寄存器的值右移8位，复制到al寄存器中。接着，将dx和al寄存器的值通过out指令输出到磁盘控制器的端口，用于设置要读取的扇区的起始LBA地址的中间字节。

将0x1F5存储在dx寄存器中，然后将ebx寄存器的值右移16位，复制到al寄存器中。接着，将dx和al寄存器的值通过out指令输出到磁盘控制器的端口，用于设置要读取的扇区的起始LBA地址的高字节。

将0x1F7存储在dx寄存器中，然后将0x20存储在al寄存器中。接着，将dx和al寄存器的值通过out指令输出到磁盘控制器的端口，用于发送读取扇区命令。

进入next_sector标签，将eax寄存器的值压入堆栈中。将 eax 压入栈中，以备后续使用。然后，使用 try_again 标签来检查磁盘是否准备好了。当 test al,8 指令的结果为 0 时，表示磁盘准备好了。此时，将 ecx 设置为 256，表示要读取 256 个扇区的数据，然后使用 rep insw 指令从端口 0x1F0 读取数据，并将数据存储在内存中。
rep insw 是一条 x86 汇编指令，用于从一个 16 位的 I/O 端口读取数据并将它们存储到内存中。该指令使用一个叫做重复前缀的前缀，表示该指令要被重复执行。在这里，rep 前缀告诉 CPU 在执行 insw 指令时，重复执行该指令，直到 ecx 寄存器中的计数器变为 0。

具体来说，insw 指令会从指定的 I/O 端口读取一个 16 位的数据，并将其存储到以 esi 寄存器指向的内存地址处，然后 esi 寄存器将递增 2 个字节，以便指向下一个内存地址。ecx 寄存器的值将递减 1，表示还有多少个数据需要读取。这个过程将重复执行，直到 ecx 的值变为 0。
out dx,al 是一条 x86 汇编指令，用于向一个 8 位的 I/O 端口输出数据。该指令将寄存器 al 中的 8 位数据写入到指定的 I/O 端口，端口号存储在寄存器 dx 中。

具体来说，out dx,al 指令将端口地址存储在 dx 寄存器中，并将要输出的 8 位数据存储在 al 寄存器中。然后该指令向指定的 I/O 端口输出 al 寄存器中的数据，即将 al 中的数据写入到端口 dx 中。

在上述代码中，out dx.al 指令将 eax 寄存器的最低 8 位数据（即 al）写入到端口地址存储在 dx 寄存器中的 I/O 端口。这个指令的作用是向 ATA 控制器发送一个命令。具体来说，该指令向端口 0x1F6 中输出 eax 寄存器中的最低 8 位数据，即发送一个 ATA 命令给磁盘。

 rep insw
rep insw 是一条 x86 汇编指令，它用于从 I/O 端口读取 16 位的数据，并将其存储到内存中。 rep 前缀指示指令将重复执行，insw 指令读取 16 位的数据并将其存储到地址寄存器中指定的地址中。在这个特定的代码中，rep insw 将读取磁盘控制器端口的数据并将其存储到内存中。

.try_again:
    mov dx,0x1f7
    in al,dx
    test al,8
    jz .try_again

    mov ecx,256
    mov dx,0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ret 
test al, 8 的作用是测试 AL 寄存器的第四个比特位是否被设置为1。在此情况下，这个比特位表示“数据传输准备好”（Data Transfer Ready，DTR）的状态，如果为1，则表示数据传输已准备好。

mov ecx, 256: 将循环计数器ECX设置为256，即要读取的字节数。
mov dx, 0x1F0: 将I/O端口地址0x1F0（即磁盘控制器的数据端口）加载到DX寄存器中，以便在接下来的指令中使用它。
rep insw: 循环执行指令insw，它的作用是从I/O端口DX读取一个字（16位），并将其存储在ES:DI指定的内存位置中。在这个例子中，ES和DI寄存器没有被显式地设置，因此默认情况下它们指向DS和BX寄存器中的地址。
pop ecx: 从堆栈中弹出保存在之前的指令中的ECX寄存器的值。由于循环指令rep insw每次迭代都会将ECX递减2，因此在这里需要将其还原为最初的值256。
loop .next_sector: 循环指令loop用于根据CX寄存器中的值跳转到目标标签（这里是.next_sector）。在每次迭代中，它都会自动将CX减1，直到它达到0为止，然后跳出循环。

address 是输出段VMA（虚拟内存地址）的表达式

gcc 参数
—I -l  (小写的l)参数就是用来指定程序要链接的库，-l参数紧接着就是库名
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

-g：生成调试信息，以便在调试时使用。
-ffreestanding：编译一个没有标准库的程序。这个选项告诉gcc不要使用标准库，而是仅使用系统提供的库。
-falign-jumps、-falign-functions、-falign-labels、-falign-loops：这些选项告诉gcc对程序进行对齐，从而提高程序执行的效率。
-fstrength-reduce：优化循环和其它计算密集型代码。
-fomit-frame-pointer：省略函数调用的帧指针，从而减小程序的大小。
-finline-functions：内联函数，从而减少函数调用带来的开销。
-Wno-unused-function、-Wno-unused-label、-Wno-unused-parameter：禁止编译器产生未使用变量的警告。
-fno-builtin：禁止使用gcc自带的内置函数。
-Werror：把所有的警告都视为错误，从而在编译时阻止产生警告。
-Wno-cpp：禁止预处理器产生警告。
-nostdlib：禁用标准库。
-nostartfiles：禁用启动文件。
-nodefaultlibs：禁用默认库。
-Wall：启用所有警告。
-O0：不进行优化。
-Iinc：指定头文件路径。

gdb调试
add-symbol-file ./build/kernelfull.o 0x100000
target remote | qemu-system-x86_64 -hda ./bin/os.bin -gdb stdio -S
break kernel_main

0xB800是显示器输出的地方，有两个字节第一个是输出的ascll，第二个是颜色、

IDT
中断描述符表（Interrupt Descriptor Table，简称IDT）和中断向量表（Interrupt Vector Table，简称IVT）是操作系统中用于处理中断的重要数据结构。

IDT是在系统启动时由操作系统初始化的，它包含了处理器能够识别和响应的所有中断类型的描述符。每个中断类型对应一个描述符，该描述符中包含了处理该中断的处理程序的入口地址等相关信息。

IVT是一块位于物理内存中的表，它是由硬件设备初始化的，用于存储中断处理程序的地址。在Intel x86架构中，IVT位于内存地址0x0000到0x03FF，共256个中断向量。当硬件设备触发一个中断时，处理器会查找对应的中断向量，在IVT中获取中断处理程序的地址，然后跳转到该地址执行中断处理。

因此，IDT和IVT都是处理中断的重要数据结构，但它们有着不同的作用和实现方式。IDT是由操作系统初始化的，用于存储处理中断的描述符，而IVT是由硬件设备初始化的，用于存储中断处理程序的地址

中断描述符的结构:
中断处理程序的偏移地址（Offset）：中断处理程序在内存中的地址，用于处理中断的具体操作。
代码段选择器（Segment Selector）：用于指定处理中断的代码段在全局描述符表（Global Descriptor Table，简称GDT）中的索引号。
属性（Attribute）：用于指定中断处理程序的属性，如中断类型、特权级等。
保留位（Reserved）：保留字段，未使用。
struct idt_entry {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
};
offset_low和offset_high字段组成了中断处理程序在内存中的偏移地址，即IDT指向的中断处理程序的入口地址。它们共同组成了一个16位的中断处理程序地址，由于x86是基于小端字节序的，所以offset_low存储地址的低16位，offset_high存储地址的高16位。
segment_selector字段指定了中断处理程序所在的代码段在GDT中的选择子（Selector）。在x86架构中，选择子是一个16位的值，由一个索引和特权级（即Ring）组成，它指向GDT中的一个段描述符，用于描述代码段或数据段的基地址、大小和属性等信息。
type_attr字段指定了中断处理程序的类型和属性，包括中断类型、特权级、允许或禁止中断等属性。
zero字段保留，未使用。

Interrupt Gate是x86架构中的一种中断描述符类型，用于处理硬件中断或软件中断。它的属性字段（Type Attribute）的值为0x8E。在该类型的中断描述符中，中断处理程序被存储在内存中的某个位置，并且在触发中断时，CPU会自动跳转到该中断处理程序的入口地址，从而响应中断请求。

中断门（Interrupt Gate）的类型和属性字段如下：

0~3位：中断处理程序的偏移地址（Offset）低16位。
4~15位：中断处理程序所在的代码段选择子（Segment Selector）。
16~31位：保留字段，必须设置为0。
32~35位：属性字段（Type Attribute），包括以下几个部分：
32位：中断处理程序类型（D）。该位为0表示中断处理程序是16位的，为1表示中断处理程序是32位的。
33~34位：特权级（Descriptor Privilege Level，DPL）。表示中断处理程序所在的代码段的特权级别，即中断处理程序的运行级别。它可以是0、1、2或3，其中0表示最高特权级，3表示最低特权级。在执行中断门时，CPU会检查中断处理程序的DPL和当前代码段的特权级别是否符合要求，如果不符合则会产生一个“通用保护故障”（General Protection Fault）异常。
35位：允许中断（Present）。该位表示中断门是否存在，如果为1，则中断门有效，可以响应中断请求；如果为0，则中断门无效，不能响应中断请求。
36~47位：中断处理程序的偏移地址（Offset）高16位。
中断门是x86架构中用于响应中断请求的关键机制之一，它的作用是为每个中断请求设置一个中断处理程序，并且在发生中断时跳转到该中断处理程序的入口地址，从而完成中断处理的功能。

__attribute__((packed))可以用于取消编译器的字节对齐操作，从而使用最小的字节对齐方式来压缩结构体的大小

在IDT Entry的属性字段type_attr中，用一个8位的字节表示中断门的类型、特权级别和是否存在等信息。其中，前四位表示中断门的类型和特权级别，后四位表示一些其他属性。在这个例子中，desc->type_attr的值为0xEE，它的二进制形式为11101110，按位解析如下：

0xE
Bit 0：表示中断门是否存在，这里为1，表示存在。
Bit 1-2：表示中断门的特权级别（DPL），这里为0b011，表示内核级别。
Bit 3 保留位数
Bit 4-8：表示中断门的类型，这里为0b1110，表示中断门类型。

Offset: A 32-bit value, split in two parts. It represents the address of the entry point of the Interrupt Service Routine.
Selector: A Segment Selector with multiple fields which must point to a valid code segment in your GDT.
Gate Type: A 4-bit value which defines the type of gate this Interrupt Descriptor represents. There are five valid type values:
0b0101 or 0x5: Task Gate, note that in this case, the Offset value is unused and should be set to zero.
0b0110 or 0x6: 16-bit Interrupt Gate
0b0111 or 0x7: 16-bit Trap Gate
0b1110 or 0xE: 32-bit Interrupt Gate
0b1111 or 0xF: 32-bit Trap Gate
这有一个保留字段0
DPL: A 2-bit value which defines the CPU Privilege Levels which are allowed to access this interrupt via the INT instruction. Hardware interrupts ignore this mechanism.
P: Present bit. Must be set (1) for the descriptor to be valid.

insb:
    push ebp
    mov ebp, esp
这两条指令将当前栈帧的 ebp 值压栈，并将 ebp 的值设置为当前栈顶地址。
    xor eax, eax
    mov edx, [ebp+8]
    in al, dx
这三条指令将0存入寄存器 eax，将 ebp+8 的值（即 insb 函数调用时的第一个参数）加载到寄存器 edx，然后从 dx 中读取1个字节的输入数据并存储到 al 寄存器中。
    pop ebp
    ret
这两条指令将栈顶值赋值给 ebp，然后从栈顶弹出值，并将弹出的值作为返回地址跳转回调用 insb 函数的地方。
insw同理不过将al 改为了ax
outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, al

    pop ebp
    ret
    这段代码实现了 outb 函数，将 ebp+12 的值（即 outb 函数调用时的第二个参数）加载到 eax 寄存器中，将 ebp+8 的值（即 outb 函数调用时的第一个参数）加载到 edx 寄存器中，然后将 al 中的1个字节数据输出到 dx 端口。

    可屏蔽中断的代理—可编程中断控制器 8259A。

8259A的作用是负责所有外来设备的中断，其中就包括来自时钟的中断，以后我们要通过它完成进程调度。


HEAP 
c programing language 
char *ptr = (char*)(0x100000);

memory limits for a 32 bit kernel
allow us to malloc arrdess to a maximum of 4.29GB

32 bit in operating system  is byte so the memory is 2^35  

memory of an uninitialized system
video memory takes up portions of ram
hardware memorry takes up pinters of ram
unused parts of ram are available for use
note that :0xc0000000 is reserved this means memory array we have at 0x1000000 can give us a maximum of 3.22gb for a machine with 4gb or gigher installed

heap 
can be  pointed at an address by hardware that is also big enough for us to use
the heap data size can be defined for example 100MB of heap memory
so long as we have 100MB of memory available  our heap will work fine
we need will be responsible for storig information in our kernel
the heap implementation will be responsible for managinng this giant memory that we call the heap

simplest possible heap implemention
start with a sstart address and call it a current address point it somewhere free i.e(0x1000000) 
any call to malloc gets the current address stores it in a temporary varible called tmp
now the current address is incremented by the size provided to malloc
temporary varible called tmp that contains the allocated address is returned 
current_arress now contains the next address for malloc to return when malloc is called again 

这种实现方式非常简单，但也有一些缺点，例如不能回收已分配的内存，也没有考虑内存碎片问题。

heap implemention 
will consist of a gaint table which describles againts piece of free memory in the system .this table will describle which memory is taken,which memory is free and so on.we will call this the entry table

will have another pointer to a giant piece of free memory this will be the actual heap data its self that users of malloc can use.we will call this data pool if our heap can allocate 100MB of ram then the heap data pool will be 100MB in size

our heap imolementation will be block based each address returne from malloc will be aligned to 4096 in size

the entry table
composed of an array of 1 byte values that represent an entryin our heap data pool

array size is calculated  by taking the hep data pool size and dividing it by our block size of 4096 bytes

100MB /4096 = 25600 bytes

the entry structure 
HAS_N IS_FIRST 0 0 ET_3 ET_2 ET_1 ET_0
HAS_N set if the entry to the right of us is part of our allocation
is_first set if is the first entry of our allocation

entry types 
heap_block_table_entry_taken the entry is taken  and the address cannot be used
heap_block_table_entry_free
the entry is free and may be used

malloc step1 finding the total blocks
step2 find two free blocks in the table
step3 calculating the absolute address thr programing can use

picture which discrebs paging bit information in detail

paging directory entry 
the bits of entry between 11 and  31 is page table 4 kb aligned address
PAT（Page Attribute Table）：如果系统支持PAT，则此位用于指示内存缓存类型；否则，保留且必须设置为0。
G（Global）：告诉处理器不要在MOV CR3指令时失效与页面对应的TLB条目。CR4的第7位（PGE）必须设置才能启用全局页面。
PS（Page Size）：存储特定页表项所对应的页的大小。如果该位被设置，则PDE映射到一个大小为4 MiB的页；否则，映射到一个4 KiB的页表。请注意，4 MiB的页面需要启用PSE。
D（Dirty）：用于确定页面是否被写入。
A（Accessed）：用于发现在虚拟地址转换期间是否读取了PDE或PTE。如果已经读取，则设置该位；否则，不设置。请注意，CPU不会清除此位，因此如果操作系统需要此位，则需要操作系统清除该位。
PCD（Cache Disable）：如果该位被设置，则该页不会被缓存；否则，该页将被缓存。
PWT（Page Write-Through）：控制页面的写入方式。如果该位被设置，则启用写入穿透缓存；如果没有设置，则启用写回缓存。
U/S（User/Supervisor）：控制根据特权级别访问页面的权限。如果该位被设置，则所有人都可以访问页面；如果未设置，则仅监管者可以访问。对于一个页面目录项，用户位控制所有由页面目录项引用的页面的访问。因此，如果要将页面设置为用户页面，则必须在相关页面目录项和页面表项中设置用户位。
R/W（Read/Write）：控制页面的读/写权限标志。如果该位被设置，则页面是可读/写的。否则，如果未设置，则页面是只读的。CR0中的WP位决定是否仅应用于用户空间，始终为内核提供写访问权限（默认情况下），或同时为用户空间和内核提供写访问权限。
P（Present）：如果该位被设置，则页面实际上在物理内存中；例如，当页面被交换出时，它不在物理内存中，因此不是“存在”的。如果调用了一个未“存在”的页面，则会发生页错误，操作系统应该处理它。


在x86架构中，CR3是一个控制寄存器，用于存储分页机制的页目录表的地址。在操作系统中，需要使用该控制寄存器来实现虚拟内存到物理内存的映射。

hard port
I/O端口号	端口用途	端口位数
主控制器	从控制器	读取操作	写入操作
0x1f0	0x170	Data	Data	16
0x1f1	0x171	Error	Features	8
0x1f2	0x172	Sector count	Sector count	8
0x1f3	0x173	LBA low	LBA low	8
0x1f4	0x174	LBA mid	LBA mid	8
0x1f5	0x175	LBA high	LBA high	8
0x1f6	0x176	Device	Device	8
0x1f7	0x177	Status	Command	8