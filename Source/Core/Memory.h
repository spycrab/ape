#pragma once
//! \file

#include <vector>

#include "Common/Types.h"

#include "Core/CPU/Exception.h"

namespace Core
{
//! Wrapper around emulated RAM
namespace Memory
{
//! Get the contents of RAM
std::vector<u8>& Get();

//! Converts a virtual address to an absolute one
u32 VirtToPhys(u16 segment, u16 offset);

template <typename T> T& Get(u16 segment, u16 offset)
{
  u32 address = VirtToPhys(segment, offset);

  if (address + sizeof(T) >= Get().size())
    throw Core::CPU::MemoryOutOfRangeException();

  return *reinterpret_cast<T*>(&Get()[address]);
}

template <typename T> T* GetPtr(u16 segment, u16 offset)
{
  return &Get<T>(segment, offset);
}
} // namespace Memory
} // namespace Core
