// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

using namespace Core::CPU;

void CPU::STOSB(const Instruction& ins)
{
  m_memory.Get<u8>(ES, DI) = AL;

  DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
}

void CPU::CMPSB(const Instruction& ins)
{
  do {
    u8 dst = m_memory.Get<u8>(DS, SI);
    u8 src = m_memory.Get<u8>(ES, SI);
    u8 cmp = dst - src;

    // LOG("Comparing " + String::ToHex(dst) + " (" + String::ToHex(DS) + ":"
    // +
    //    String::ToHex(SI) + ") with " + String::ToHex(src) + " (" +
    //    String::ToHex(ES) + ":" + String::ToHex(DI) + ")");

    UpdateSF(cmp);
    UpdateZF(cmp);
    UpdatePF(cmp);
    UpdateOF<i8>(cmp);
    UpdateCF<i8>(cmp);

    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
  } while (HandleRepetition());
}

void CPU::LODSB(const Instruction& ins)
{
  do {
    AL = m_memory.Get<u8>(DS, SI);

    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
  } while (HandleRepetition());
}

void CPU::MOVSB(const Instruction& ins)
{
  do {
    u8& dst = m_memory.Get<u8>(ES, DI);
    u8 src = m_memory.Get<u8>(PrefixToValue(ins.GetPrefix()), SI);

    dst = src;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
  } while (HandleRepetition());
}

void CPU::MOVSW(const Instruction& ins)
{
  do {
    u16& dst = m_memory.Get<u16>(ES, DI);
    u16 src = m_memory.Get<u16>(PrefixToValue(ins.GetPrefix()), SI);

    dst = src;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
  } while (HandleRepetition());
}