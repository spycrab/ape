// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Common/Logger.h"

#include <iostream>
#include <string>

void __MSG(std::string type, std::string file, int line, std::string msg)
{
  auto StripPath = [](std::string file) {
    static const std::string src_prefix = "Source/";
    auto index = file.find(src_prefix);

    if (index == std::string::npos)
      return file;

    return file.substr(index + src_prefix.size());
  };

  std::cout << "[" << type << " " << StripPath(file) << ":" << line << "] "
            << msg << std::endl;
}
