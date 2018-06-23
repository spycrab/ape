// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <string>

namespace Util
{
namespace File
{
//! Check whether a given file exists
bool Exists(const std::string& path);
}

namespace Path
{
//! Turn DOS/Windows paths into *nix friendly paths
std::string ToUnix(const std::string& path);
}
}
