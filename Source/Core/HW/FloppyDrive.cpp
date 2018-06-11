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

  m_file->read(reinterpret_cast<char*>(buffer), size);

  return m_file->good();
}

bool Core::HW::FloppyDrive::Read(u8 cylinder, u8 head, u8 sector, u8 count,
                                 u8* buffer)
{
  const auto sector_size = GetSectorSize();
  const auto total_sector =
      (cylinder * GetHeadCount() + head) * GetSectorsPerTrack() + (sector - 1);

  LOG("Sector = " + String::ToHex<u32>(total_sector));
  LOG("Address = " + String::ToHex<u32>(total_sector * sector_size));

  return Read(total_sector * sector_size, count * sector_size, buffer);
}

void Core::HW::FloppyDrive::Eject() { m_file.reset(); }

// This is presuming a 360K 5.25" floppy disc
u32 Core::HW::FloppyDrive::GetSectorSize() const { return 512; }
u32 Core::HW::FloppyDrive::GetSectorsPerTrack() const { return 9; }
u32 Core::HW::FloppyDrive::GetHeadCount() const { return 2; }
