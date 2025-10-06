# chip-8

CHIP-8 interpreter written in C using SDL2.

## prerequisites

* [SDL2](https://www.libsdl.org/) is used to render the graphics and handle the input.
* [curl](https://curl.se/) is used to pull data about ROMs from the [chip-8-database](https://github.com/chip-8/chip-8-database).
* [openSSL](https://www.openssl.org/) is used to get SHA1 hashes of ROMs.

### installation

#### macOS
```bash
brew install sdl2 curl openssl
```

#### Ubuntu
```bash
sudo apt-get install libsdl2-dev libcurl4-openssl-dev libssl-dev
```

#### Arch Linux
```bash
sudo pacman -S sdl2 curl openssl
```

## compile

```bash
git clone https://github.com/jacob-thompson/chip-8.git
cd chip-8
make
```

## run

```bash
./chip8 <rom> <rate=1000>
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
