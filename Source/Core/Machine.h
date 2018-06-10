// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include "Core/CPU/CPU.h"
#include "Core/HW/FloppyDrive.h"
#include "Core/Memory.h"

#include <vector>

namespace Core
{
//! Representation of a PC
class Machine
{
public:
  explicit Machine();

  //! Get this machines HW::FloppyDrive
  HW::FloppyDrive& GetFloppyDrive();

  //! Get this machines Memory
  Memory& GetMemory();

  //! Boot the machine
  bool Boot();

private:
  HW::FloppyDrive m_floppy_drive;
  CPU::CPU m_cpu;

  Core::Memory m_memory;
};
} // namespace Core
