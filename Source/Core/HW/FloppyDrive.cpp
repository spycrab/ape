// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/HW/FloppyDrive.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/HW/DiskFormats.h"

#include <iostream>
#include <map>

namespace Core::HW::FloppyDrive
{
const DiskFormat* m_format = nullptr;

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
  static const std::map<size_t, DiskFormat> formats = {
      {163'840, {PhysicalFormat::IBM_5_25_INCH_FLOPPY, 8, 512, 1}},
      {184'320, {PhysicalFormat::IBM_5_25_INCH_FLOPPY, 9, 512, 1}},
      {368'640, {PhysicalFormat::IBM_5_25_INCH_FLOPPY, 9, 512, 2}},
      {1'228'800, {PhysicalFormat::IBM_5_25_INCH_FLOPPY, 15, 512, 2}},
      {1'474'560, {PhysicalFormat::IBM_3_5_INCH_FLOPPY, 18, 512, 2}}};

  if (formats.count(GetSize()) == 0) {
    ERROR("Unknown format");
    return false;
  }

  m_format = &formats.at(GetSize());

  LOG("Guessing this is a " + PhysicalFormatToString(m_format->physical) +
      " with " + std::to_string(GetSize() / 1024) + "K of capacity");

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

u32 GetSectorSize() { return m_format == nullptr ? 0 : m_format->sector_size; }
u32 GetSectorsPerTrack()
{
  return m_format == nullptr ? 0 : m_format->sectors_per_track;
}
u32 GetHeadCount() { return m_format == nullptr ? 0 : m_format->head_count; }
} // namespace Core::HW::FloppyDrive
