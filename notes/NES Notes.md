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

### Zero Page

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

