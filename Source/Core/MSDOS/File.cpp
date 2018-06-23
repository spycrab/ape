// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/MSDOS/File.h"

#include <fstream>
#include <map>

#include "Common/File.h"
#include "Common/Logger.h"
#include "Common/String.h"

using namespace Core::MSDOS;

static std::map<HFile, std::fstream> s_handles;

// TODO: Don't ignore mode
std::optional<HFile> File::Open(const std::string& path, u8 mode)
{
  LOG("Path: " + path);
  LOG("Mode: " + String::ToHex(mode));

  const auto& unix_path = Util::Path::ToUnix(path);

  if (!Util::File::Exists(unix_path)) {
    LOG("Requested file " + unix_path + " does not exist");
    return std::nullopt;
  }

  for (size_t handle = 0; handle < 0xFFFF; handle++) {
    if (s_handles.count(handle))
      continue;

    LOG("Got handle for " + unix_path + ": " + String::ToHex<u16>(handle));

    s_handles[static_cast<u16>(handle)] =
        std::move(std::fstream(unix_path, std::ios::binary | std::ios::in));
    return static_cast<u16>(handle);
  }

  ERROR("Too many file handles! Can't open new file.");

  throw std::nullopt;
}
