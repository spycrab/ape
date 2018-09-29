// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Common/File.h"

#include <fstream>

using namespace Util;

bool File::Exists(const std::string& path)
{
  std::ifstream ifs(path);
  return ifs.good();
}

std::string Path::ToUnix(const std::string& path)
{
  std::string unix_path = path;

  for (size_t index = unix_path.find('\\'); index != -1;
       index = unix_path.find('\\')) {
    unix_path[index] = '/';
  }

  return unix_path;
}
