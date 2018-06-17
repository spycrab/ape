// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/CPU/Exception.h"

using namespace Core::CPU;

void CPU::AND(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord()) {
    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());

    dst16 |= src16;

    UpdateSF(dst16);
    UpdateZF(dst16);
    UpdatePF(dst16);
  } else {
    u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
    u8& src8 = ParameterTo<u8&>(src, ins.GetPrefix());

    dst8 |= src8;

    UpdateSF(dst8);
    UpdateZF(dst8);
    UpdatePF(dst8);
  }
}

void CPU::OR(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord() != src.IsWord())
    throw ParameterLengthMismatchException();

  if (dst.IsWord()) {
    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16 src16 = ParameterTo<u16>(src, ins.GetPrefix());

    dst16 |= src16;
    OF = CF = 0;

    UpdateSF(dst16);
    UpdateZF(dst16);
    UpdatePF(dst16);
  } else {
    u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
    u8 src8 = ParameterTo<u8>(src, ins.GetPrefix());

    dst8 |= src8;
    OF = CF = 0;

    UpdateSF(dst8);
    UpdateZF(dst8);
    UpdatePF(dst8);
  }
}

void CPU::ROL(const Instruction& ins) {}
void CPU::ROR(const Instruction& ins) {}

void CPU::SHL(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  u8 shift = ParameterTo<u8>(src, ins.GetPrefix());

  if (dst.IsWord()) {
    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());

    OF = CF = (dst16 << (shift - 1)) >> (sizeof(u16) * 8 - shift - 1);
    dst16 <<= shift;

    UpdatePF(dst16);
    UpdateZF(dst16);
    UpdateSF(static_cast<i16>(dst16));
  } else {
    u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());

    OF = CF = (dst8 << (shift - 1)) >> (sizeof(u8) * 8 - shift - 1);
    dst8 <<= shift;

    UpdatePF(dst8);
    UpdateZF(dst8);
    UpdateSF(static_cast<i8>(dst8));
  }
}

void CPU::SHR(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  u8 shift = ParameterTo<u8>(src, ins.GetPrefix());

  if (dst.IsWord()) {
    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());

    OF = CF = (dst16 >> (shift - 1)) << (sizeof(u16) * 8 - shift - 1);
    dst16 >>= shift;

    UpdatePF(dst16);
    UpdateZF(dst16);
    UpdateSF(static_cast<i16>(dst16));
  } else {
    u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());

    OF = CF = (dst8 >> (shift - 1)) << (sizeof(u8) * 8 - shift - 1);
    dst8 >>= shift;

    UpdatePF(dst8);
    UpdateZF(dst8);
    UpdateSF(static_cast<i8>(dst8));
  }
}

void CPU::XOR(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord() != src.IsWord()) {
    throw ParameterLengthMismatchException();
  }

  if (dst.IsWord()) {
    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16 src16 = ParameterTo<u16>(src, ins.GetPrefix());
    dst16 ^= src16;

    UpdateSF(dst16);
    UpdateZF(dst16);
    UpdatePF(dst16);
  } else {
    u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
    u8 src8 = ParameterTo<u8>(src, ins.GetPrefix());
    dst8 ^= src8;

    UpdateSF(dst8);
    UpdateZF(dst8);
    UpdatePF(dst8);
  }

  // TODO: Flags (OF, CF)!
}
