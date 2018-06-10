#include "Core/CPU/CPU.h"

using namespace Core::CPU;

void CPU::UpdateZF(u16 value) { ZF = (value == 0); }

void CPU::UpdatePF(u16 value) { PF = (value % 2 == 0); }

void CPU::UpdateSF(i16 value) { SF = value < 0; }

template <typename T> void CPU::UpdateOF(i32 value)
{
  OF = value < std::numeric_limits<T>().min() ||
       value > std::numeric_limits<T>().ma();
}

// TODO: Check if this actually works properly.
template <typename T> void CPU::UpdateCF(i32 value)
{
  // Check if there has been a carry or borrow from the last byte
  CF = value & (sizeof(T) * 8);
}
