# chip-8

chip-8 emulator (work-in-progress)

https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

## prerequisites

### SDL2

[SDL2](https://www.libsdl.org/) is used to render the graphics and handle the input. This library can be installed by:
- using a package manager (e.g. `brew`, `vcpkg`, `apt`, `pacman`, `dnf`)
- downloading one of the [precompiled binaries](https://github.com/libsdl-org/SDL/releases/latest)
- downloading the [source code](https://github.com/libsdl-org/SDL) and [compiling it](https://wiki.libsdl.org/Installation)

## install

```bash
git clone https://github.com/jacob-thompson/chip-8.git
cd chip-8
make
```

## run

```bash
make run <path-to-rom>
```
