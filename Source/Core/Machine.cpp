// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/Machine.h"

#include "Common/Logger.h"

#include "Core/TTY.h"

Core::Machine::Machine() : m_memory(1024 * 1024), m_vga(this), m_cpu(this)
{
  TTY::Init(&m_vga);
}

Core::HW::FloppyDrive& Core::Machine::GetFloppyDrive()
{
  return m_floppy_drive;
}

Core::HW::VGACard& Core::Machine::GetVGA() { return m_vga; }

Core::Memory& Core::Machine::GetMemory() { return m_memory; }

bool Core::Machine::BootFloppy()
{
  if (!m_floppy_drive.Read(0, 512, m_memory.GetPtr<u8>(0x0000, 0x7C00)))
    return false;

  m_cpu.CS = 0;
  m_cpu.IP = 0x7C00;

  m_cpu.Start();

  return true;
}

void Core::Machine::Stop() { m_cpu.running = false; }

void Core::Machine::Pause() { m_cpu.paused = !m_cpu.paused; }

bool Core::Machine::BootCOM(const std::string& file,
                            const std::string&& parameters)
{
  std::ifstream ifs(file, std::ios::binary);

  if (!ifs.good())
    return false;

  m_cpu.DS = 0;
  m_cpu.IP = 0x100;
  m_cpu.simulate_msdos = true;

  size_t index;
  for (index = 0; !ifs.eof(); index++) {
    m_memory.Get<u8>(0x0000, 0x0100 + index) = static_cast<u8>(ifs.get());
  }

  LOG("Loaded " + std::to_string(index - 1) + " bytes into memory");

  m_memory.Get<u8>(0x0000, 0x0080) = parameters.size();

  u16 offset;

  for (offset = 0x0081; offset < parameters.length(); offset++)
    m_memory.Get<char>(0x0000, offset) = parameters[offset - 0x0081];

  m_memory.Get<char>(0x0000, offset) = '\0';

  LOG("Command line parameters are \"" + parameters + "\"");

  ifs.close();

  m_cpu.Start();

  return true;
}
