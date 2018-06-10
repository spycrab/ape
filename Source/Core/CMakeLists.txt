add_library(Core
  CPU/CPU.cpp
  CPU/Decoder.cpp
  CPU/Flags.cpp
  CPU/Instruction.cpp
  CPU/Interrupt.cpp
  HW/FloppyDrive.cpp
  Machine.cpp
  Memory.cpp)
  
source_group(CPU FILES   
  CPU/CPU.cpp
  CPU/Decoder.cpp
  CPU/Flags.cpp
  CPU/Instruction.cpp
  CPU/Interrupt.cpp)

source_group(HW FILES   
  HW/FloppyDrive.cpp)