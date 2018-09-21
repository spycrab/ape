// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

using namespace Core;

void CPU::STOSB(const Instruction&)
{
  do {
    Memory::Get<u8>(ES, DI) = AL;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
  } while (HandleRepetition());
}

void CPU::STOSW(const Instruction&)
{
  do {
    Memory::Get<u16>(ES, DI) = AX;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
  } while (HandleRepetition());
}

void CPU::CMPSB(const Instruction&)
{
  do {
    u8 dst = Memory::Get<u8>(DS, SI);
    u8 src = Memory::Get<u8>(ES, SI);
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

void CPU::CMPSW(const Instruction&)
{
  do {
    u16 dst = Memory::Get<u16>(DS, SI);
    u16 src = Memory::Get<u16>(ES, SI);
    u16 cmp = dst - src;

    // LOG("Comparing " + String::ToHex(dst) + " (" + String::ToHex(DS) + ":"
    // +
    //    String::ToHex(SI) + ") with " + String::ToHex(src) + " (" +
    //    String::ToHex(ES) + ":" + String::ToHex(DI) + ")");

    UpdateSF(cmp);
    UpdateZF(cmp);
    UpdatePF(cmp);
    UpdateOF<i16>(cmp);
    UpdateCF<i16>(cmp);

    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
  } while (HandleRepetition());
}

void CPU::LODSB(const Instruction&)
{
  do {
    AL = Memory::Get<u8>(DS, SI);

    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
  } while (HandleRepetition());
}

void CPU::LODSW(const Instruction&)
{
  do {
    AX = Memory::Get<u16>(DS, SI);

    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
  } while (HandleRepetition());
}

void CPU::MOVSB(const Instruction& ins)
{
  do {
    u8& dst = Memory::Get<u8>(ES, DI);
    u8 src = Memory::Get<u8>(PrefixToValue(ins.GetPrefix()), SI);

    dst = src;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
  } while (HandleRepetition());
}

void CPU::MOVSW(const Instruction& ins)
{
  do {
    u16& dst = Memory::Get<u16>(ES, DI);
    u16 src = Memory::Get<u16>(PrefixToValue(ins.GetPrefix()), SI);

    dst = src;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
    SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
  } while (HandleRepetition());
}
