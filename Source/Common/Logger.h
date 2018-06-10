// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

//! \file
#pragma once

#include <iostream>
#include <string>

//! \cond PRIVATE
[[maybe_unused]] static void __MSG(std::string type, std::string file, int line,
                                   std::string msg)
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
//! \endcond PRIVATE

//! Log a message
#define LOG(msg) __MSG("LOG", __FILE__, __LINE__, msg)
//! Log a warning
#define WARN(msg) __MSG("WARNING", __FILE__, __LINE__, msg)
//! Log an error
#define ERROR(msg) __MSG("ERROR", __FILE__, __LINE__, msg)
