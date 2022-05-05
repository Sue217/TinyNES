# Realization of NES's CPU Simulator with C++

## DAY 01

**Simulate 6502 CPU, MainBus and Memory RAM to realize CPU reading and writing data and fetching instructions from memory.**

## DAY 02

**Implemented decoding and execution of instructions by 6502 CPU.**

## DAY 03

**Test our previous code for bugs and if the CPU would work properly.**

**Part of code is test code which will be commented later**

NES software writing example, located in the `example/nes` directory, mainly including files:

- `nes.cfg`: Specified the link method
- `example.asm`： Implement a simple "4+2" operation


### 1. Download the compiler

- **MacOS:** `brew install cc65`
- [Linux](https://debian.trikaliotis.net/cc65/)
- [Windows](https://sourceforge.net/projects/cc65/files/cc65-snapshot-win32.zip)

### 2. Compile & Link
```shell
ca65 example.asm -o example.o 
ld65 -C nes.cfg example.o -o example.nes
```

### 3. Check `example.nes` file by binary code

Install `hexdump for vscode` under vscode, and open it with the binary reader, you will see:

- 0x0000: header of `.nes`

- 0x0010:

    ```assembly
    A9 04 ; LDA #$4
    69 02 ; ADC #$2
    ```

- 0x8000：Interrupt vector table saves the target address `0x8000`, because we specified `PRG: start = $8000` in **example/nes/nes.cfg** , which is the first address of the `main()` in our assembly code.

    ```assembly
    00000000: 4E 45 53 1A 02 00 00 00 00 00 00 00 00 00 00 00    NES.............
    00000010: A9 04 69 02 00 00 00 00 00 00 00 00 00 00 00 00
    ...
    ...
    00008000: 00 00 00 00 00 00 00 00 00 00 00 80 00 80 00 80
    ```

    

### 4. Compile and Execute the simulator

```shell
make
./NES
```

### 5. Check the result

The result is as follows:

```shell
Reading ROM from path: ./example/example.nes
Reading header, it dictates:
16KB PRG-ROM Banks: 2
8KB CHR-ROM Banks: 0
ROM is NTSC compatible.
a9 4 69 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Cartridge with CHR-RAM.
MainBus Read a Byte: 0
MainBus Read a Byte: 80
[+]After reset, PC locates: 0x8000
MainBus Read a Byte: a9
MainBus Read a Byte: 4
MainBus Read a Byte: 69
MainBus Read a Byte: 2
[+]After executing "4+2" operation, the value of ACC is: 6
```
