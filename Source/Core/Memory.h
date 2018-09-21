#pragma once
//! \file

#include <vector>

#include "Common/Types.h"

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
  return *reinterpret_cast<T*>(&Get()[VirtToPhys(segment, offset)]);
}

template <typename T> T* GetPtr(u16 segment, u16 offset)
{
  return &Get<T>(segment, offset);
}
} // namespace Memory
} // namespace Core
