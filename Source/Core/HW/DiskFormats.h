// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <string>

#include "Common/Types.h"

namespace Core::HW
{
enum class PhysicalFormat : u8 {
  IBM_8_INCH_FLOPPY,
  IBM_5_25_INCH_FLOPPY,
  IBM_3_5_INCH_FLOPPY
};

std::string PhysicalFormatToString(const PhysicalFormat& f);

struct DiskFormat {
  const PhysicalFormat physical;
  //! Amount of sectors per track
  const u16 sectors_per_track;
  //! Size of sector in bytes
  const u16 sector_size;
  //! Amount of heads (i.e. the amount of sides a disk has)
  const u16 head_count;
};
}; // namespace Core::HW
