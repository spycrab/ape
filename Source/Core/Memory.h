#pragma once
//! \file

#include <vector>

#include "Common/Types.h"

namespace Core
{
//! Wrapper around emulated RAM
class Memory
{
public:
  //! Create Memory of the specified size (in bytes)
  Memory(u32 size);

  //! Get the contents of RAM
  std::vector<u8>& Get();

  template <typename T> T& Get(u16 segment, u16 offset)
  {
    return *reinterpret_cast<T*>(&Get()[VirtToPhys(segment, offset)]);
  }

  //! Converts a virtual address to an absolute one
  static u32 VirtToPhys(u16 segment, u16 offset);

private:
  std::vector<u8> m_data;
};
} // namespace Core
