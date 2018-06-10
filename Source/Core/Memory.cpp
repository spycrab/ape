#include "Core/Memory.h"

Core::Memory::Memory(u32 size) { m_data.resize(size); }

std::vector<u8>& Core::Memory::Get() { return m_data; }

u32 Core::Memory::VirtToPhys(u16 segment, u16 offset)
{
  return (segment << 4) + offset;
}
