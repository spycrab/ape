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
//! Attempts to create a new MSDOS file handle
std::optional<HFile> Open(const std::string& path, u8 mode);
}
}
