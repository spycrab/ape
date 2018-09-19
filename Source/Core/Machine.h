// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include <vector>

#include "Core/CPU/CPU.h"
#include "Core/HW/FloppyDrive.h"
#include "Core/HW/VGA.h"
#include "Core/Memory.h"

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

  //! Get this machines graphics adapter
  HW::VGACard& GetVGA();

  //! Boot the machine from the floppy drive
  bool BootFloppy();

  //! Shutdown the machine
  void Shutdown();

  //! Pause the machine (Or unpause it if it's paused already)
  void Pause();

  //! Directly execute a COM file
  bool BootCOM(const std::string& file, const std::string&& parameters = "");

private:
  HW::FloppyDrive m_floppy_drive;
  HW::VGACard m_vga;
  CPU::CPU m_cpu;

  Core::Memory m_memory;
};
} // namespace Core
