add_library(Core
  BIOS/Interrupt.cpp
  Core.h
  Core.cpp
  CPU/CPU.h
  CPU/CPU.cpp
  CPU/Breakpoint.h
  CPU/Breakpoint.cpp
  CPU/Decoder.cpp
  CPU/Exception.h
  CPU/Exception.cpp
  CPU/Flags.h
  CPU/Flags.cpp
  CPU/Instruction.h
  CPU/Instruction.cpp
  CPU/Instructions/Arithmetic.cpp
  CPU/Instructions/Bitwise.cpp
  CPU/Instructions/Jumps.cpp
  CPU/Instructions/String.cpp
  CPU/Interrupt.cpp
  HW/DiskFormats.h
  HW/DiskFormats.cpp
  HW/FloppyDrive.h
  HW/FloppyDrive.cpp
  HW/VGA.h
  HW/VGA.cpp
  Memory.h
  Memory.cpp
  MSDOS/File.cpp
  MSDOS/Interrupt.cpp
  TTY.cpp
  TTY.h)

target_link_libraries(Core PRIVATE
  Common)

source_group(BIOS FILES
  BIOS/Interrupt.cpp)

source_group(CPU FILES
  CPU/Breakpoint.h
  CPU/Breakpoint.cpp
  CPU/CPU.h
  CPU/CPU.cpp
  CPU/Decoder.cpp
  CPU/Exception.h
  CPU/Exception.cpp
  CPU/Flags.h
  CPU/Flags.cpp
  CPU/Instruction.h
  CPU/Instruction.cpp
  CPU/Interrupt.cpp)

source_group("CPU\\Instructions" FILES
  CPU/Instructions/Arithmetic.cpp
  CPU/Instructions/Bitwise.cpp
  CPU/Instructions/Jumps.cpp
  CPU/Instructions/String.cpp)

source_group(HW FILES
  HW/DiskFormats.h
  HW/DiskFormats.cpp
  HW/FloppyDrive.h
  HW/FloppyDrive.cpp
  HW/VGA.h
  HW/VGA.cpp)

source_group(Core FILES
  Core.h
  Core.cpp)

source_group(Memory FILES
  Memory.h
  Memory.cpp)

source_group(MSDOS FILES
  MSDOS/File.cpp
  MSDOS/Interrupt.cpp)

source_group(TTY FILES
  TTY.cpp
  TTY.h)
