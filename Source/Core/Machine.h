// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include <string>

//! Representation of a PC
namespace Core::Machine
{
void Init();

//! Boot the machine from the floppy drive
bool BootFloppy();

//! Stop the machine
void Stop();

//! Pause the machine (Or unpause it if it's paused already)
void Pause();

//! Directly execute a COM file
bool BootCOM(const std::string& file, const std::string&& parameters = "");
} // namespace Core::Machine
