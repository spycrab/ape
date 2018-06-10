#pragma once

#include "Core/CPU/CPU.h"

using namespace Core::CPU;

template <typename T> void CPU::UpdateOF(i32 value)
{
  OF = value < std::numeric_limits<T>().min() ||
       value > std::numeric_limits<T>().max();
}

// TODO: Check if this actually works properly.
template <typename T> void CPU::UpdateCF(i32 value)
{
  // Check if there has been a carry or borrow from the last byte
  CF = value & (sizeof(T) * 8);
}
