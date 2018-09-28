# Ape
Ape (Another PC Emulator) is a free / open source IBM PC compatible emulator written in C++17.

**If you're looking for a usable, user-friendly or even complete Emulator, Ape is not for you. DOSBox or dosemu might be what you are looking for.**

## Status
Currently Ape has the following limitations:

- *A lot* of bugs
- Not all instructions are implemented
- Not all interrupts are implemented
- Limited hardware support
  - Limited to 8086/8088 instructions only
  - No Intel 8087 emulation
  - No Harddrive support
- Limited graphics capabilities
  - Only plain text
  - Basic VGA text mode support

## Requirements
- A C++17 capable compiler
- [CMake](https://cmake.org/)
- [Qt 5](https://www.qt.io) (5.9 or newer)
- [Doxygen](http://www.doxygen.nl/) (*Docs only*)
- [GoogleTest](https://github.com/google/googletest/) (*Tests only*)

## Documentation

You can either check out a semi-recent version of the documentation [online](https://spycrab.github.io/ape-docs-live/) (Also available as a [PDF](https://spycrab.github.io/ape-docs-live/docs.pdf)).  

..or build the most recent version yourself using the steps listed below.

## Building

## *nix (Linux, macOS, *BSD etc.)
```sh
mkdir build && cd build
cmake ..
```

```sh
make Docs # Documentation
make Disasm # Disassembler
make ApeCLI # Emulator without an UI
make ApeQt # Emulator with a Qt5 UI

make all # All of the above

make tests # Run the testsuite
```

## Windows
Either use MinGW or Cygwin to get a POSIX-like environment or use CMake GUI to generate a Visual Studio Project.


## License
Ape is licensed under the GNU General Public License v3 (or any later version). See [LICENSE](LICENSE).
