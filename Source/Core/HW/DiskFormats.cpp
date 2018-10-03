// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/HW/DiskFormats.h"

namespace Core::HW
{
std::string PhysicalFormatToString(const PhysicalFormat& f)
{
  switch (f) {
  case PhysicalFormat::IBM_3_5_INCH_FLOPPY:
    return "IBM 3.5\" floppy disk";
  case PhysicalFormat::IBM_5_25_INCH_FLOPPY:
    return "IBM 5.25\" floppy disk";
  case PhysicalFormat::IBM_8_INCH_FLOPPY:
    return "IBM 8\" floppy disk";
  default:
    return "Unknown";
  }
}
} // namespace Core::HW
