#include "Core/Memory.h"

using namespace Core;

static std::vector<u8> m_data(1024 * 1024);

std::vector<u8>& Memory::Get() { return m_data; }

u32 Memory::VirtToPhys(u16 segment, u16 offset)
{
  return segment * 0x10 + offset;
}
