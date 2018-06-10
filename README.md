# Ape
Ape (Another PC Emulator) is a free / open source IBM PC compatible emulator written in C++17.

**If you're looking for a usable, user-friendly or even complete Emulator, Ape is not for you. DOSBox or dosemu might be what you are looking for.**

## Status
Currently Ape has the following limitations:

- Limited hardware support
  - Not all instructions are implemented
  - Limited to 8086/8088 instructions only
  - Not all interrupts are implemented
  - No Harddrive support
  - No Intel 8087 emulation
- Limited graphics capabilities
  - currently only outputting to a text file (obviously temporary)


## Requirements
- A C++17 capable compiler
- CMake
- Doxygen (*Docs only*)
- GoogleTest (*Tests only*)

## Documentation
There is currently no live version of the documentation available.
You have to build it yourself using the steps mentioned above.

## Building

## *nix (Linux, macOS, *BSD etc.)
```sh
mkdir build && cd build
cmake ..
```

```sh
make Docs # Documentation
make Disasm # Disassembler
make Ape # Actual emulator

make all # All of the above

make test # Run the testsuite
```

## Windows
Either use MinGW or Cygwin to get a Posix-like environment
or use CMake GUI to generate a Visual Studio Project.


## License
Ape is licensed under the GNU General Public License v3 (or any later version). See [LICENSE](LICENSE).