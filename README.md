# teal8

CHIP-8 interpreter written in pure C.

## prerequisites

* [make](https://www.gnu.org/software/make/) is used for compilation.
* [SDL2](https://www.libsdl.org/) is used to render the graphics and handle the input.
* [curl](https://curl.se/) is used to pull data about ROMs from the [chip-8-database](https://github.com/chip-8/chip-8-database).
* [openSSL](https://www.openssl.org/) is used to get SHA1 hashes of ROMs.

#### macOS

```bash
brew install make sdl2 curl openssl
```

#### Ubuntu

```bash
sudo apt-get install make libsdl2-dev libcurl4-openssl-dev libssl-dev
```

#### Arch Linux

```bash
sudo pacman -S make sdl2 curl openssl
```

## compile

```bash
git clone https://github.com/jacob-thompson/teal8.git
cd teal8
make
```

The `teal8` binary will be in `bin/`.

## usage

You can add the program to your PATH:

```bash
export PATH="path/to/teal8/bin:$PATH"
```

Run the program:

```bash
teal8 <rom> <rate=1000> <flags>
```

You can omit the rom's file extension:

```bash
teal8 roms/spockpaperscissors
```

> [!NOTE]
> - `<rom>` is the path to the ROM file
> - `<rate>` is the amount of instructions to execute per second (default: 1000)
> - `<flags>` are optional flags (see below)

The following flags are available:

```
--mute (-m)        Mute sound
--force (-f)       Force run ROM even if not recognized
--help (-h)        Show help message
--version (-v)     Show version
```

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
