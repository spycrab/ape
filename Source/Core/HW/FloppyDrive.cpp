// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/HW/FloppyDrive.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include <iostream>

namespace Core::HW::FloppyDrive
{
u16 m_sectors_per_track;
u16 m_sector_size;
u16 m_head_count;

std::unique_ptr<std::ifstream> m_file;

bool Insert(const std::string& path)
{
  m_file.reset(new std::ifstream(path, std::ios::binary));

  if (!m_file->good())
    return false;

  if (!GuessFormat())
    return false;

  return true;
}

bool HasDisc() { return m_file != nullptr; }

u32 GetSize()
{
  m_file->seekg(0, std::ios_base::end);
  return static_cast<u32>(m_file->tellg());
}

bool GuessFormat()
{
  std::string type;
  switch (GetSize()) {
  case 163840:
    type = "IBM 5.25\"";
    m_sectors_per_track = 8;
    m_sector_size = 512;
    m_head_count = 1;
    break;
  case 184320:
    type = "IBM 5.25\"";
    m_sectors_per_track = 9;
    m_sector_size = 512;
    m_head_count = 1;
    break;
  case 368640:
    type = "IBM 5.25\"";
    m_sectors_per_track = 9;
    m_sector_size = 512;
    m_head_count = 2;
    break;
  case 1228800:
    type = "IBM 5.25\"";
    m_sectors_per_track = 15;
    m_sector_size = 512;
    m_head_count = 2;
    break;
  case 1474560:
    type = "IBM 3.5\"";
    m_sectors_per_track = 18;
    m_sector_size = 512;
    m_head_count = 2;
    break;
  default:
    ERROR("Unknown format");
    return false;
    break;
  }

  LOG("Guessing this is a " + type + " floppy disc with " +
      std::to_string(GetSize() / 1000) + "K of capacity");

  return true;
}

bool IsBootable()
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

bool Read(u32 offset, u32 size, u8* buffer)
{
  if (!HasDisc())
    return false;

  m_file->seekg(offset);

  m_file->read(reinterpret_cast<char*>(buffer), size);

  return m_file->good();
}

bool Read(u8 cylinder, u8 head, u8 sector, u8 count, u8* buffer)
{
  const auto sector_size = GetSectorSize();
  const auto total_sector =
      (cylinder * GetHeadCount() + head) * GetSectorsPerTrack() + (sector - 1);

  LOG("Sector = " + String::ToHex<u32>(total_sector));
  LOG("Address = " + String::ToHex<u32>(total_sector * sector_size));

  return Read(total_sector * sector_size, count * sector_size, buffer);
}

void Eject() { m_file.reset(); }

// This is presuming a 360K 5.25" floppy disc
u32 GetSectorSize() { return m_sector_size; }
u32 GetSectorsPerTrack() { return m_sectors_per_track; }
u32 GetHeadCount() { return m_head_count; }
} // namespace Core::HW::FloppyDrive
