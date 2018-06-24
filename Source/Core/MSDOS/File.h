// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <optional>
#include <string>

#include "Common/Types.h"

namespace Core::MSDOS
{
using HFile = u16;

namespace File
{
enum class SeekOrigin : u8 { START = 0, CUR_POS = 1, END = 2 };

//! Attempts to create a new MSDOS file handle
std::optional<HFile> Open(const std::string& path, u8 mode);

//! Attempts to seek to the given position
std::optional<u32> Seek(HFile handle, SeekOrigin origin, u32 offset);

//! Attempts to read count bytes and store them in dst
std::optional<u16> Read(HFile handle, u16 count, u8* dst);

} // namespace File
} // namespace Core::MSDOS
