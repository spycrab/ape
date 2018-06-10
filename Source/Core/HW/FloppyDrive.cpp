// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/HW/FloppyDrive.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include <iostream>

bool Core::HW::FloppyDrive::Insert(const std::string& path)
{
  m_file.reset(new std::ifstream(path, std::ios::binary));

  return m_file->good();
}

bool Core::HW::FloppyDrive::HasDisc() const { return m_file != nullptr; }

bool Core::HW::FloppyDrive::IsBootable()
{
  if (!HasDisc())
    return false;

  m_file->seekg(510);

  u16 signature;
  m_file->read(reinterpret_cast<char*>(&signature), 2);

  if (!m_file->good())
    return false;

  return signature == 0xAA55;
}

bool Core::HW::FloppyDrive::Read(u32 offset, u32 size, u8* buffer)
{
  if (!HasDisc())
    return false;

  m_file->seekg(offset);

  for (u32 i = 0; i < size; i++) {
    buffer[i] = static_cast<u8>(m_file->get());
  }
  // m_file->read(reinterpret_cast<char*>(buffer), size);

  return m_file->good();
}

void Core::HW::FloppyDrive::Eject() { m_file.reset(); }
