// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/Machine.h"

Core::Machine::Machine() : m_cpu(this), m_memory(1024 * 1024) {}

Core::HW::FloppyDrive& Core::Machine::GetFloppyDrive()
{
  return m_floppy_drive;
}

Core::Memory& Core::Machine::GetMemory() { return m_memory; }

bool Core::Machine::Boot()
{
  if (!m_floppy_drive.Read(0, 512, &(m_memory.Get()[0x7C00])))
    return false;

  m_cpu.CS = 0;
  m_cpu.IP = 0x7C00;

  m_cpu.Start();

  return true;
}
