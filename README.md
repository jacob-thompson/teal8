# chip-8

chip-8 interpreter written in C

## prerequisites

### SDL2

[SDL2](https://www.libsdl.org/) is used to render the graphics and handle the input. This library can be installed by:
- using a package manager (e.g. `brew`, `apt`, `pacman`, `dnf`)
- downloading one of the [precompiled binaries](https://github.com/libsdl-org/SDL/releases/latest)
- downloading the [source code](https://github.com/libsdl-org/SDL) and [compiling it](https://wiki.libsdl.org/Installation)

## compile

```bash
git clone https://github.com/jacob-thompson/chip-8.git
cd chip-8
make
```

## run

```bash
./chip8 <rom> <rate>
```

For roms in the `roms/` directory, you can omit the rom's file path and extension:

```bash
./chip8 spockpaperscissors
```

> [!NOTE]
> - `<rom>` is the path to the ROM file
> - `<rate>` is the rate at which instructions should be executed (in Hz)
>   * The default and maximum instruction rate is 1000 Hz
>   * The instruction rate is rounded to the nearest divisor of 1000 Hz (e.g. 500 Hz, 250 Hz, 125 Hz, ...)

## controls

The controls are mapped to the following keys:

```
1 2 3 4
Q W E R
A S D F
Z X C V
```

## special thanks

- [tobiasvl](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Timendus](https://github.com/Timendus/chip8-test-suite)
- [johnearnest](https://johnearnest.github.io/chip8Archive/)
