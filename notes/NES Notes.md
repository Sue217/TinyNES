# NES Notes

## Sources

nesdev wiki https://wiki.nesdev.com/w/index.php/Nesdev#NES
NES-PPU https://blog.csdn.net/qq_34254642/article/details/104193445
Nintendo Entertainment System Architecture http://fms.komkon.org/EMUL8/NES.html
wiki_nesdev_标准控制器 https://wiki.nesdev.com/w/index.php/Standard_controller
MCS6502 https://github.com/bzotto/MCS6502

## MCS6502

When the 6502 starts up it loads an address vector from 0xFFFC/0xFFFD, sets its program counter to that address, and start fetching instructions from there. That means at a bare minimum you need to return useful data when asked for values at those addresses and return some code when the CPU starts asking for bytes at the address in the vector. The vector addresses are defined in the header file for your convenience.

In a real hardware configuration, a system typically ties those high addresses to a ROM chip where the reset vector is hard-coded and points to an address (also within ROM) where the system start up code is.

You should know that the 6502 privileges the first ("zero page") of 256 bytes (0x00xx) as a special page for fast-access work area and the second page (0x01xx) is always the stack. The rest is up to you. The 6502 was super popular in real life and remains super popular in emulation, so there are great detailed resources all over the place for learning more.

## Zero Page

零页或基本页是计算机地址空间最开始的内存块。即起始地址为零的页面。页的大小取决于上下文，零页内存与较高寻址内存的意义高度取决于机器体系结构。例如，Motorola 6800和 **MOS Technology 6502** 处理器系列专门处理内存的前256字节，而许多其他处理器则不这样做。

### General Architecture

NES is based on the 6502 CPU and a custom video controller known as PPU (Picture Processing Unit). The PPU's video memory is separated from the main CPU memory and can be read/written via special ports.

Cartridges may contain both ROM appearing in the CPU address space at $8000-$FFFF, and VROM or VRAM appearing in the PPU address space at $0000-$1FFF.

In smaller cartridges, which only have 16kB ROM, it takes place at $C000-$FFFF leaving $8000-$BFFF area unused. In cartridges with >32kB ROM, it is paged into address space with special circuitry (see "[Mappers](http://fms.komkon.org/EMUL8/NES.html#LABK)"). Some cartridges also have RAM at $6000-$7FFF which may or may not be battery-backed.

Cartridge VROM (VRAM) is used for Pattern Tables (aka Tile Tables, Character Generators, etc.). The usual amount is 8kB which contain 2 Pattern Tables. In cartridges with >8kB VROM (VRAM), it is paged into address space with special circuitry (see "[Mappers](http://fms.komkon.org/EMUL8/NES.html#LABK)").

Internal NES VRAM is located at addresses $2000-$3FFF in the PPU memory and used to store Name Tables (aka Screen Buffers, etc.). Although PPU supports 4 Name Tables, there is only enough memory for two of them. Other two mirror the first two (see "[PPU Details](http://fms.komkon.org/EMUL8/NES.html#LABH)").

### CPU Memory Map

```
--------------------------------------- $10000
 Upper Bank of Cartridge ROM
--------------------------------------- $C000
 Lower Bank of Cartridge ROM
--------------------------------------- $8000
 Cartridge RAM (may be battery-backed)
--------------------------------------- $6000
 Expansion Modules
--------------------------------------- $5000
 Input/Output
--------------------------------------- $2000
 2kB Internal RAM, mirrored 4 times
--------------------------------------- $0000
```

```cpp
/* 
 * Stack pointer starts at top of page 1, but lands at 0xFD 
 * (rather than 0xFF) after the hardware startup is complete.
*/
```

![img](https://upload-images.jianshu.io/upload_images/22600211-4717d138a5a16961.png?imageMogr2/auto-orient/strip|imageView2/2/w/1200/format/webp)

![img](https://upload-images.jianshu.io/upload_images/22600211-5beca677bb85f64a.png?imageMogr2/auto-orient/strip|imageView2/2/w/812/format/webp)

1. **0x0000 - 0x0800** ( RAM )

    这是主机中 2KB RAM 的数据，分成了 3 块

- **0x0000 - 0x00FF** ( Zero page )
     前 256 字节划分为 Zero page，这块内存相比其他区域不同点在于能让 CPU 以更快的速度访问，所以需要频繁读写的数据会优先放入此区域
- **0x0100 - 0x01FF** ( Stack )
     这一块区域用于栈数据的存储，SP（栈指针） 从 0x1FF 处向下增长
- **0x0200 - 0x07FF** ( 剩余 RAM )
     这是 2KB 被 Zero page 和 Sack 瓜分后剩余的区域

2. **0x0800 - 0x2000** ( Mirrors )
     你可能会感觉到奇怪这个 Mirror 到底是干什么的。实际上它是 0x0000 - 0x07FF 数据的镜像，总共重复 3 次 例如：0x0001, 0x0801, 0x1001, 0x1801 都指向了同样的数据，用程序来解释的话，就是： `address &= 0x07FF` 对应到硬件上的话，就是 bit11 - 13 的线不接
    至于为什么任天堂要这样设计？我猜可能是考虑到成本原因，2KB RAM 够用了，不需要更大的 RAM，但是地址空间得用完啊，所以才有了 Mirror 效果

3. **0x2000 - 0x401F** ( IO Registers )
    这里包含了部分外设的数据，包括 PPU，APU，输入设备的寄存器。比如 CPU 如果想读写 VRAM 的数据，就得靠 PPU 寄存器作为中介

4. **0x4020 - 0x5FFF** ( Expansion ROM )
    Nesdev 的论坛上有篇解释这块区域的[帖子](https://links.jianshu.com/go?to=https%3A%2F%2Fforums.nesdev.com%2Fviewtopic.php%3Ff%3D3%26t%3D12936)，简单讲，该区域用于一些 Mapper 扩展用，大部分情况用不到

5. **0x6000 - 0x7FFF** ( SRAM )
    这就是之前说过的带电池的 RAM 了，该区域位于卡带上

6. **0x8000 - 0xFFFF** ( Program ROM )
     这里对应了程序的数据，一般 CPU 就在这块区域中执行指令，该区域位于卡带上

https://www.jianshu.com/p/77e075a3a442
来源：简书

![img](https://upload-images.jianshu.io/upload_images/22600211-48219508e51ac08c.png?imageMogr2/auto-orient/strip|imageView2/2/w/774/format/webp)

## 🌟 CPU 的本质，只有两件事：

1. 什么时候执行什么指令
2. 什么时候读写什么地址的数据

CPU 有一个时钟作为输入源，该时钟实际上只是一个频率很高的脉冲波，一般几 M 到 几 GHZ。传统的 CPU 会在一个到多个时钟期间执行完一条指令，然后再执行下一条指令。如果某一时刻产生了中断，CPU 会读取中断向量表对应中断地址，等到当前指令执行完后切换到该地址处继续执行

[NES CPU](https://www.nesdev.org/wiki/CPU) 为 6502 指令集，型号为 RP2A03，CPU 时钟 1.79 MHz



2A03 有 6 个寄存器：A，X，Y，PC，SP，P，**除了 PC 为 16bit 以外，其他全都是 8bit**

- A
     通常作为累加器
- X，Y
     通常作为循环计数器
- PC
     程序计数器，记录下一条指令地址
- SP
     堆栈寄存器，其值为 0x00 ~ 0xFF，对应着 CPU 总线上的 0x100 ~ 0x1FF
- P
     标志寄存器比较麻烦，它实际上只有 6bit，但是我们可以看成 8bit

### Accumulator

**A** is byte-wide and along with the [arithmetic logic unit](https://en.wikipedia.org/wiki/arithmetic_logic_unit) (ALU), supports using the status register for carrying, overflow detection, and so on.

### Indexes

**X** and **Y** are byte-wide and used for several addressing modes. They can be used as loop counters easily, using INC/DEC and branch instructions. Not being the accumulator, they have limited addressing modes themselves when loading and saving.

### Program Counter

The 2-byte program counter **PC** supports 65536 direct (unbanked) memory locations, however not all values are sent to the cartridge. It can be accessed either by allowing CPU's internal fetch logic increment the address bus, an interrupt (NMI, Reset, IRQ/BRQ), and using the RTS/JMP/JSR/Branch instructions.

### Stack Pointer

**S** is byte-wide and can be accessed using interrupts, pulls, pushes, and transfers.

### Status Register

**P** has 6 bits used by the ALU but is byte-wide. PHP, PLP, arithmetic, testing, and branch instructions can access this register.

| BIT  | 名称 |                   含义                   |
| :--: | :--: | :--------------------------------------: |
|  0   |  C   |  进位标志，如果计算结果产生进位，则置 1  |
|  1   |  Z   |     零标志，如果结算结果为 0，则置 1     |
|  2   |  I   | 中断去使能标志，置 1 则可屏蔽掉 IRQ 中断 |
|  3   |  D   |            十进制模式，未使用            |
|  4   |  B   |              BRK，后面解释               |
|  5   |  U   |             未使用，后面解释             |
|  6   |  V   | 溢出标志，如果结算结果产生了溢出，则置 1 |
|  7   |  N   |     负标志，如果计算结果为负，则置 1     |



刚说过标志寄存器只有 6 bit，这是因为 **B 和 U 并不是实际位**，只不过某些指令执行后，标志位 push 到 stack 的时候，会**附加上这两位以区分中断是由 BRK 触发还是 IRQ 触发**，下面是详细解释



| 指令或中断 | U 和 B 的值 | push 之后对 P 的影响 |
| :--------: | :---------: | :------------------: |
|  PHP 指令  |     11      |          无          |
|  BRK 指令  |     11      |        I 置 1        |
|  IRQ 中断  |     10      |        I 置 1        |
|  MNI 中断  |     10      |        I 置 1        |



http://www.oxyron.de/html/opcodes02.html

有个测试文件对于 CPU 开发非常有帮助：
 下载地址：[http://www.qmtpro.com/~nes/misc/](https://links.jianshu.com/go?to=http%3A%2F%2Fwww.qmtpro.com%2F~nes%2Fmisc%2F)
 下载这 2 个文件：`nestest.log` 和 `nestest.nes`

开发 CPU 时，先将 PC 设置为 0xC000，再让 CPU 运行，每运行一条指令后，和它的 nestest.log 对照一下各个寄存器状态，这样能按照它的 log 顺序逐条开发出正确的指令，比一口气开发完再去 debug 好得多

## instructions

### Logical and arithmetic commands:

| Opcode | imp  | imm  | zp   | zpx  | zpy  | izx  | izy  | abs  | abx  | aby  | ind  | rel  | Function             | N    | V    | B    | D    | I    | Z    | C    |
| ------ | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | -------------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| ORA    |      | $09  | $05  | $15  |      | $01  | $11  | $0D  | $1D  | $19  |      |      | A:=A or {adr}        | *    |      |      |      |      | *    |      |
| AND    |      | $29  | $25  | $35  |      | $21  | $31  | $2D  | $3D  | $39  |      |      | A:=A&{adr}           | *    |      |      |      |      | *    |      |
| EOR    |      | $49  | $45  | $55  |      | $41  | $51  | $4D  | $5D  | $59  |      |      | A:=A exor {adr}      | *    |      |      |      |      | *    |      |
| ADC    |      | $69  | $65  | $75  |      | $61  | $71  | $6D  | $7D  | $79  |      |      | A:=A+{adr}           | *    | *    |      |      |      | *    | *    |
| SBC    |      | $E9  | $E5  | $F5  |      | $E1  | $F1  | $ED  | $FD  | $F9  |      |      | A:=A-{adr}           | *    | *    |      |      |      | *    | *    |
| CMP    |      | $C9  | $C5  | $D5  |      | $C1  | $D1  | $CD  | $DD  | $D9  |      |      | A-{adr}              | *    |      |      |      |      | *    | *    |
| CPX    |      | $E0  | $E4  |      |      |      |      | $EC  |      |      |      |      | X-{adr}              | *    |      |      |      |      | *    | *    |
| CPY    |      | $C0  | $C4  |      |      |      |      | $CC  |      |      |      |      | Y-{adr}              | *    |      |      |      |      | *    | *    |
| DEC    |      |      | $C6  | $D6  |      |      |      | $CE  | $DE  |      |      |      | {adr}:={adr}-1       | *    |      |      |      |      | *    |      |
| DEX    | $CA  |      |      |      |      |      |      |      |      |      |      |      | X:=X-1               | *    |      |      |      |      | *    |      |
| DEY    | $88  |      |      |      |      |      |      |      |      |      |      |      | Y:=Y-1               | *    |      |      |      |      | *    |      |
| INC    |      |      | $E6  | $F6  |      |      |      | $EE  | $FE  |      |      |      | {adr}:={adr}+1       | *    |      |      |      |      | *    |      |
| INX    | $E8  |      |      |      |      |      |      |      |      |      |      |      | X:=X+1               | *    |      |      |      |      | *    |      |
| INY    | $C8  |      |      |      |      |      |      |      |      |      |      |      | Y:=Y+1               | *    |      |      |      |      | *    |      |
| ASL    | $0A  |      | $06  | $16  |      |      |      | $0E  | $1E  |      |      |      | {adr}:={adr}*2       | *    |      |      |      |      | *    | *    |
| ROL    | $2A  |      | $26  | $36  |      |      |      | $2E  | $3E  |      |      |      | {adr}:={adr}*2+C     | *    |      |      |      |      | *    | *    |
| LSR    | $4A  |      | $46  | $56  |      |      |      | $4E  | $5E  |      |      |      | {adr}:={adr}/2       | *    |      |      |      |      | *    | *    |
| ROR    | $6A  |      | $66  | $76  |      |      |      | $6E  | $7E  |      |      |      | {adr}:={adr}/2+C*128 | *    |      |      |      |      | *    | *    |



### Move commands:

| Opcode | imp  | imm  | zp   | zpx  | zpy  | izx  | izy  | abs  | abx  | aby  | ind  | rel  | Function | N    | V    | B    | D    | I    | Z    | C    |
| ------ | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | -------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| LDA    |      | $A9  | $A5  | $B5  |      | $A1  | $B1  | $AD  | $BD  | $B9  |      |      | A:={adr} | *    |      |      |      |      | *    |      |
| STA    |      |      | $85  | $95  |      | $81  | $91  | $8D  | $9D  | $99  |      |      | {adr}:=A |      |      |      |      |      |      |      |
| LDX    |      | $A2  | $A6  |      | $B6  |      |      | $AE  |      | $BE  |      |      | X:={adr} | *    |      |      |      |      | *    |      |
| STX    |      |      | $86  |      | $96  |      |      | $8E  |      |      |      |      | {adr}:=X |      |      |      |      |      |      |      |
| LDY    |      | $A0  | $A4  | $B4  |      |      |      | $AC  | $BC  |      |      |      | Y:={adr} | *    |      |      |      |      | *    |      |
| STY    |      |      | $84  | $94  |      |      |      | $8C  |      |      |      |      | {adr}:=Y |      |      |      |      |      |      |      |
| TAX    | $AA  |      |      |      |      |      |      |      |      |      |      |      | X:=A     | *    |      |      |      |      | *    |      |
| TXA    | $8A  |      |      |      |      |      |      |      |      |      |      |      | A:=X     | *    |      |      |      |      | *    |      |
| TAY    | $A8  |      |      |      |      |      |      |      |      |      |      |      | Y:=A     | *    |      |      |      |      | *    |      |
| TYA    | $98  |      |      |      |      |      |      |      |      |      |      |      | A:=Y     | *    |      |      |      |      | *    |      |
| TSX    | $BA  |      |      |      |      |      |      |      |      |      |      |      | X:=S     | *    |      |      |      |      | *    |      |
| TXS    | $9A  |      |      |      |      |      |      |      |      |      |      |      | S:=X     |      |      |      |      |      |      |      |
| PLA    | $68  |      |      |      |      |      |      |      |      |      |      |      | A:=+(S)  | *    |      |      |      |      | *    |      |
| PHA    | $48  |      |      |      |      |      |      |      |      |      |      |      | (S)-:=A  |      |      |      |      |      |      |      |
| PLP    | $28  |      |      |      |      |      |      |      |      |      |      |      | P:=+(S)  | *    | *    |      | *    | *    | *    | *    |
| PHP    | $08  |      |      |      |      |      |      |      |      |      |      |      | (S)-:=P  |      |      |      |      |      |      |      |



### Jump/Flag commands:

| Opcode | imp  | imm  | zp   | zpx  | zpy  | izx  | izy  | abs  | abx  | aby  | ind  | rel  | Function               | N    | V    | B    | D    | I    | Z    | C    |
| ------ | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---------------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| BPL    |      |      |      |      |      |      |      |      |      |      |      | $10  | branch on N=0          |      |      |      |      |      |      |      |
| BMI    |      |      |      |      |      |      |      |      |      |      |      | $30  | branch on N=1          |      |      |      |      |      |      |      |
| BVC    |      |      |      |      |      |      |      |      |      |      |      | $50  | branch on V=0          |      |      |      |      |      |      |      |
| BVS    |      |      |      |      |      |      |      |      |      |      |      | $70  | branch on V=1          |      |      |      |      |      |      |      |
| BCC    |      |      |      |      |      |      |      |      |      |      |      | $90  | branch on C=0          |      |      |      |      |      |      |      |
| BCS    |      |      |      |      |      |      |      |      |      |      |      | $B0  | branch on C=1          |      |      |      |      |      |      |      |
| BNE    |      |      |      |      |      |      |      |      |      |      |      | $D0  | branch on Z=0          |      |      |      |      |      |      |      |
| BEQ    |      |      |      |      |      |      |      |      |      |      |      | $F0  | branch on Z=1          |      |      |      |      |      |      |      |
| BRK    | $00  |      |      |      |      |      |      |      |      |      |      |      | (S)-:=PC,P PC:=($FFFE) |      |      | 1    |      | 1    |      |      |
| RTI    | $40  |      |      |      |      |      |      |      |      |      |      |      | P,PC:=+(S)             | *    | *    |      | *    | *    | *    | *    |
| JSR    |      |      |      |      |      |      |      | $20  |      |      |      |      | (S)-:=PC PC:={adr}     |      |      |      |      |      |      |      |
| RTS    | $60  |      |      |      |      |      |      |      |      |      |      |      | PC:=+(S)               |      |      |      |      |      |      |      |
| JMP    |      |      |      |      |      |      |      | $4C  |      |      | $6C  |      | PC:={adr}              |      |      |      |      |      |      |      |
| BIT    |      |      | $24  |      |      |      |      | $2C  |      |      |      |      | N:=b7 V:=b6 Z:=A&{adr} | *    | *    |      |      |      | *    |      |
| CLC    | $18  |      |      |      |      |      |      |      |      |      |      |      | C:=0                   |      |      |      |      |      |      | 0    |
| SEC    | $38  |      |      |      |      |      |      |      |      |      |      |      | C:=1                   |      |      |      |      |      |      | 1    |
| CLD    | $D8  |      |      |      |      |      |      |      |      |      |      |      | D:=0                   |      |      |      | 0    |      |      |      |
| SED    | $F8  |      |      |      |      |      |      |      |      |      |      |      | D:=1                   |      |      |      | 1    |      |      |      |
| CLI    | $58  |      |      |      |      |      |      |      |      |      |      |      | I:=0                   |      |      |      |      | 0    |      |      |
| SEI    | $78  |      |      |      |      |      |      |      |      |      |      |      | I:=1                   |      |      |      |      | 1    |      |      |
| CLV    | $B8  |      |      |      |      |      |      |      |      |      |      |      | V:=0                   |      | 0    |      |      |      |      |      |
| NOP    | $EA  |      |      |      |      |      |      |      |      |      |      |      |                        |      |      |      |      |      |      |      |



https://www.nesdev.org/6502_cpu.txt



```cpp
high ___________ stack
     |         |
     |         |
     |         |
     -----------
     ----PCH---- addr + 1
     ----PCL---- addr
low  |         |
```



```cpp
Address CPU::readAddress(Address addr) {
  return m_bus.read(addr) | m_bus.read(addr + 1) << 8;
} // M[...]

Data MainBus::read(const Address& addr) const {
  if (addr < 0x2000) {
    return m_RAM[addr & 0x7ff];
  }
  return 0;
}

void MainBus::write(const Address& addr, const Data& value) {
  if (addr < 0x2000) {
    m_RAM[addr & 0x7ff] = value;
  }
}

void CPU::push(Data value) {
  m_bus.write(0x100 | r_sp, value);
  --r_sp;
}

Data CPU::pull() {
  return m_bus.read(0x100 | ++r_sp);
}
```

The accumulator really acts as two functions: 1) It is one of the primary storage points for the machine; 2) It is the point at which intermediate results are normally stored. 

If the program counter was only 1 byte and if the bit pattern which allows the microprocessor to choose which instruction it wants to act on next, such as "LDA" as opposed to an "AND", was contained in one byte of data we could only have 256 program steps. Although the machine of this length might make-an interesting toy, it would have no real practical value. Therefore, almost all of the competitive 8 bit microprocessors have chosen to go to a double length program counter. 

In the MCS650X family, the program counter is set with the value of the address of an instruction. 



## **Q:** always r_pc++, will r_pc + 1 = 0? (Page or Mapper or etc.?)

## **M:** source/CPU.cpp:355 & source/CPU.cpp:379~382

In other words, the first fetch is used to pick up the OP CODE, LDA, the second fetch is used to pick up the low order address byte of the data and the third fetch is used to pick up the high order address byte of the data. This is the form in which many of the microprocessor instructions will appear as it is the most simple form of addressing in the machine and allows referencing to any memory location.

the MCS650X uses "relative" addressing for all conditional test instructions. To perform any branch, the program counter must be changed. In rela- tive addressing, however, we add the value in the memory location following the OP CODE to the program counter. This allows us to specify a new program counter location with only two bytes, one for the OP CODE and one for the value to be added. 

SBC

This instruction affects the accumulator. The carry flag is set if the result is greater than or equal to 0. The carry flag is reset when the result is less than 0, indicating a borrow. The over- flow flag is set when the result exceeds +127 or -127, otherwise it is reset. The negative flag is set if the result in the accumulator has bit 7 on, otherwise it is reset. The Z flag is set if the result in the accumulator is 0, otherwise it is reset. 

## !DO NOT change the instructions (names) in the enum{}...

**it will cause to fetch wrong operation instructions!!!**

## 

```cpp
/*
 *        7  6  5    4  3  2    1  0
 * Data: [op op op] [ad ad ad] [im im]
 *       operation  address  instruction mode(00~10)
*/
```

**The number of times the pc is incremented by one depends on the number of times the operation is fetched from memory.**

```cpp
/*
 * 参考：https://blog.csdn.net/qq_34254642/article/details/104193445
 * NES使用两个调色板，
 * 每个有16个条目，图像调色板($3F00-$3F0F)和精灵调色板($3F10-$3F1F)。
 * 图像调色板显示当前背景块可用的颜色。精灵调色板显示精灵当前可用的颜色。这些调色板不存储实际的颜色值，而是系统调色板中的颜色索引。由于只需要64个唯一值，所以可以忽略第6和第7位。
 * 
 * 调色板条目$3F00是背景色，用于透明。
 * 使用镜像使调色板中的每四个字节是$3F00的一个副本。
 * 因此，$3F04、$3F08 $3FOC、$3F10、$3F14、$3F18和$3F1C只是$3F00的副本。
 * 每个调色板的颜色是13，而不是16。因此，在任何时候，屏幕上的颜色总数是52种颜色中的25种。两个调色板也镜像到$3F20-$3FFF。
*/


//Colors in RGBA (8 bit colors)
// red green blue alpha 格式
// 由调色板映射实际显示的颜色
```

# !!!Previous Bugs!!!

## CPU.cpp

### 1. 146 lines

### 2. 166 lines

### 3. 234 lines

### 4. 376 lines

### 5. 388 lines

### 6. 522 lines

## Emulator.cpp

### pay attention to line 77 & 78 line

### pay attention to the function run() in 81 lines
