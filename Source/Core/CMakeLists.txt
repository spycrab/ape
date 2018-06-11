add_library(Core
  CPU/CPU.h
  CPU/CPU.cpp
  CPU/Decoder.cpp
  CPU/Flags.h
  CPU/Flags.cpp
  CPU/Instruction.h
  CPU/Instruction.cpp
  CPU/Interrupt.cpp
  HW/FloppyDrive.h
  HW/FloppyDrive.cpp
  Machine.h
  Machine.cpp
  Memory.h
  Memory.cpp)

source_group(CPU FILES
  CPU/CPU.h
  CPU/CPU.cpp
  CPU/Decoder.cpp
  CPU/Flags.h
  CPU/Flags.cpp
  CPU/Instruction.h
  CPU/Instruction.cpp
  CPU/Interrupt.cpp)

source_group(HW FILES
  HW/FloppyDrive.h
  HW/FloppyDrive.cpp)

source_group(Machine FILES
  Machine.h
  Machine.cpp)

source_group(Memory FILES
  Memory.h
  Memory.cpp)
