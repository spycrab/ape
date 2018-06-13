// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

//! \file
#pragma once

#include <string>

//! \cond PRIVATE
void __MSG(std::string type, std::string file, int line, std::string msg);
//! \endcond PRIVATE

//! Log a message
#define LOG(msg) __MSG("LOG", __FILE__, __LINE__, msg)
//! Log a warning
#define WARN(msg) __MSG("WARNING", __FILE__, __LINE__, msg)
//! Log an error
#define ERROR(msg) __MSG("ERROR", __FILE__, __LINE__, msg)
