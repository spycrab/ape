// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include "Core/CPU/Exception.h"

using namespace Core::CPU;

void CPU::ADC(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord()) {
    u16& dst_16 = ParameterTo<u16&>(dst, ins.GetPrefix());

    u32 sum = dst_16;

    if (src.IsWord())
      sum += ParameterTo<u16>(src, ins.GetPrefix());
    else
      sum += ParameterTo<u8>(src, ins.GetPrefix());

    dst_16 = sum & 0xFFFF;

    CF = dst_16 & 0x10000;
    OF = dst_16 & 0x20000;

    sum &= 0b1'1111'1111'1111'1111;

    UpdateSF(static_cast<i16>(sum));
    UpdateZF(static_cast<i16>(sum));
    UpdatePF(static_cast<i16>(sum));
  } else {
    u8& dst_8 = ParameterTo<u8&>(dst, ins.GetPrefix());
    u8 src_8 = ParameterTo<u8>(src, ins.GetPrefix());

    u16 sum = dst_8 + src_8;

    CF = sum & 0x100;
    OF = sum & 0x200;

    dst_8 = sum & 0xFF;

    sum &= 0b1'1111'1111;

    UpdateSF(sum);
    UpdateZF(sum);
    UpdatePF(sum);
  }
}

void CPU::ADD(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord()) {
    i16& dst_16 = ParameterTo<i16&>(dst, ins.GetPrefix());
    i16 src_16;

    if (src.IsWord())
      src_16 = ParameterTo<i16>(src, ins.GetPrefix());
    else
      src_16 = ParameterTo<i8>(src, ins.GetPrefix());

    UpdateOF<i16>(dst_16 + src_16);
    UpdateCF<i16>(static_cast<u16>(dst_16) + static_cast<u16>(src_16));

    dst_16 += src_16;

    UpdateSF(dst_16);
    UpdateZF(dst_16);
    UpdatePF(dst_16);
  } else {
    i8& dst_8 = ParameterTo<i8&>(dst, ins.GetPrefix());
    i8 src_8 = ParameterTo<i8>(src, ins.GetPrefix());

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException(ins, dst, src);

    UpdateOF<i8>(dst_8 + src_8);
    UpdateCF<i8>(static_cast<u8>(dst_8) + static_cast<u8>(src_8));

    dst_8 += src_8;

    UpdateSF(dst_8);
    UpdateZF(dst_8);
    UpdatePF(dst_8);
  }
}

void CPU::DEC(const Instruction& ins)
{
  auto& parameter = ins.GetParameters()[0];

  if (parameter.IsWord()) {
    i16& data_i16 = ParameterTo<i16&>(parameter, ins.GetPrefix());

    UpdateOF<i16>(static_cast<i32>(data_i16) - 1);

    data_i16--;

    UpdateSF(data_i16);
    UpdateZF(data_i16);
    UpdatePF(data_i16);
  } else {
    i8& data_i8 = ParameterTo<i8&>(parameter, ins.GetPrefix());

    UpdateOF<i8>(static_cast<i32>(data_i8) - 1);

    data_i8--;

    UpdateSF(data_i8);
    UpdateZF(data_i8);
    UpdatePF(data_i8);
  }
}

void CPU::DIV(const Instruction& ins)
{
  auto& div = ins.GetParameters()[0];

  if (div.IsWord()) {
    u32 dx_ax = (DX << 16) | AX;

    const u16 divisor = ParameterTo<u16>(div, ins.GetPrefix());

    AX = static_cast<u16>(dx_ax / divisor);
    DX = dx_ax % divisor;

  } else {
    const u8 divisor = ParameterTo<u8>(div, ins.GetPrefix());
    AL = static_cast<u8>(AX / divisor);
    AH = AX % divisor;
  }
}

void CPU::INC(const Instruction& ins)
{
  auto& parameter = ins.GetParameters()[0];

  if (parameter.IsWord()) {
    i16& data_i16 = ParameterTo<i16&>(parameter, ins.GetPrefix());

    UpdateOF<i16>(static_cast<i32>(data_i16) + 1);

    data_i16++;

    UpdateSF(data_i16);
    UpdateZF(data_i16);
    UpdatePF(data_i16);

  } else {
    i8& data_i8 = ParameterTo<i8&>(parameter, ins.GetPrefix());

    UpdateOF<i8>(static_cast<i32>(data_i8) + 1);

    data_i8++;

    UpdateSF(data_i8);
    UpdateZF(data_i8);
    UpdatePF(data_i8);
  }
}

void CPU::SBB(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord()) {
    u16& dst_16 = ParameterTo<u16&>(dst, ins.GetPrefix());

    u32 sum = dst_16;

    if (src.IsWord())
      sum -= ParameterTo<u16>(src, ins.GetPrefix());
    else
      sum -= ParameterTo<u8>(src, ins.GetPrefix());

    dst_16 = sum & 0xFFFF;

    CF = dst_16 & 0x10000;
    OF = dst_16 & 0x20000;

    sum &= 0b1111'1111'1111'1111;

    UpdateSF(static_cast<i16>(sum));
    UpdateZF(static_cast<i16>(sum));
    UpdatePF(static_cast<i16>(sum));
  } else {
    u8& dst_8 = ParameterTo<u8&>(dst, ins.GetPrefix());
    u8 src_8 = ParameterTo<u8>(src, ins.GetPrefix());

    u16 sum = dst_8 - src_8;

    CF = sum & 0x100;
    OF = sum & 0x200;

    dst_8 = sum & 0xFF;

    sum &= 0b1111'1111;

    UpdateSF(static_cast<i8>(sum));
    UpdateZF(static_cast<i8>(sum));
    UpdatePF(static_cast<i8>(sum));
  }
}

void CPU::SUB(const Instruction& ins)
{
  auto& dst = ins.GetParameters()[0];
  auto& src = ins.GetParameters()[1];

  if (dst.IsWord() != src.IsWord())
    throw ParameterLengthMismatchException(ins, dst, src);

  if (dst.IsWord()) {
    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16 src16 = ParameterTo<u16>(src, ins.GetPrefix());

    UpdateOF<i16>(dst16 - src16);
    UpdateCF<i16>(dst16 - src16);

    dst16 -= src16;

    UpdateSF(dst16);
    UpdateZF(dst16);
    UpdatePF(dst16);
  } else {
    u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
    u8 src8 = ParameterTo<u8>(src, ins.GetPrefix());

    UpdateOF<i8>(dst8 - src8);
    UpdateCF<i8>(dst8 - src8);

    dst8 -= src8;

    UpdateSF(dst8);
    UpdateZF(dst8);
    UpdatePF(dst8);
  }
}
