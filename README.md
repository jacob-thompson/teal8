# CHIP-8

CHIP-8 Emulator (work-in-progress)

https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

## Pre-requisites

### SDL2

[SDL2](https://www.libsdl.org/) is a library used for rendering the graphics and handling input. This library can be installed by:
- using a package manager (e.g. `brew`, `vcpkg`, `apt`, `pacman`, `dnf`)
- downloading one of the [precompiled binaries](https://github.com/libsdl-org/SDL/releases/latest)
- downloading the [source code](https://github.com/libsdl-org/SDL) and [compiling it](https://wiki.libsdl.org/Installation)

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
