# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-10-12

### Fixed

#### Critical Bugs
- **Buffer overflow in writeFontToMemory**: Fixed critical buffer overflow where font writing loop read 512 bytes from an 80-byte array
- **Buffer overflow in writeRomToMemory**: Added bounds checking to prevent ROM overflow beyond 4KB memory limit
- **Buffer overflow in fetchOpcode**: Added bounds checking to prevent reading beyond memory when PC is at last byte
- **Memory leak in ROM reset**: Fixed file handle leak when resetting ROM during gameplay
- **Memory leaks in file.c**: Fixed multiple memory leaks in database query error paths

#### Build System
- **Makefile dependency issue**: Removed `clean` from object file dependencies which caused unnecessary rebuilds

#### Memory Safety
- **Missing break statement**: Added missing break in switch case 0x85 in emulator.c
- **NULL pointer checks**: Added NULL checks for malloc/calloc failures in multiple locations
- **Bounds checking**: Added memory bounds checking for opcodes 0x33, 0x55, 0x65, and sprite drawing

#### Input Validation
- **Division by zero**: Added validation to prevent zero or negative instruction rate
- **Empty string handling**: Added check for NULL and empty strings in isNumber function
- **Sprite memory access**: Added bounds checking for sprite drawing to prevent memory overflow
- **Font sprite calculation**: Added masking to ensure font sprite index stays within valid range (0-F)

#### Code Quality
- **Missing include guards**: Added include guards to all header files (audio.h, display.h, emulator.h, file.h, stack.h, timers.h)
- **Unsafe string functions**: Replaced sprintf with snprintf for safer string operations
- **NULL checks in display**: Added NULL pointer checks in resetDisplay and drawPixels

### Changed
- Version updated to 1.0.0
- Improved error messages and logging throughout the codebase
- Enhanced documentation with better comments explaining memory layout

### Security
- Fixed multiple buffer overflow vulnerabilities
- Added comprehensive bounds checking for memory access
- Improved input validation to prevent potential exploits
