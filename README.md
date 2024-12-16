# CHIP-8

CHIP-8 Emulator (work-in-progress)

https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

## Pre-requisites

### SDL2

[SDL2](https://www.libsdl.org/) is used for rendering the graphics and handling input. It can be installed by:
- using a package manager (e.g. `brew`, `vcpkg`, `apt-get`, `pacman`, `dnf`)
- downloading one of the [precompiled binaries](https://github.com/libsdl-org/SDL/releases/latest)
- downloading the [source code](https://github.com/libsdl-org/SDL) and [compiling it](https://wiki.libsdl.org/Installation)

#### Installing SDL2 with a package manager

MacOS:
```bash
brew install sdl2
```

Windows:
```bash
vcpkg install sdl2
```

Ubuntu:
```bash
sudo apt-get install libsdl2-dev
```

Arch:
```bash
sudo pacman -S sdl2
```

Fedora:
```bash
sudo dnf install SDL2-devel
```

## Install

```bash
git clone https://github.com/jacob-thompson/chip-8.git
cd chip-8
make
```

## Run

```bash
make run <path-to-rom>
```
