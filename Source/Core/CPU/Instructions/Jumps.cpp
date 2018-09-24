// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include "Common/Logger.h"
#include "Common/String.h"
#include "Common/Swap.h"

#include "Core/CPU/Exception.h"

using namespace Core;

using Type = CPU::Instruction::Type;
using PType = CPU::Instruction::Parameter::Type;

void CPU::JMP(const Instruction& instruction)
{
  auto& parameter = instruction.GetParameters()[0];
  switch (parameter.GetType()) {
  case PType::Literal_Offset:
    IP += parameter.GetData<i8>();
    break;
  case PType::Value_BP_Offset_Word:
  case PType::Literal_WordOffset:
    IP += ParameterTo<i16>(parameter, instruction.GetPrefix());
    break;
  case PType::Literal_LongAddress_Immediate: {
    u32 address = parameter.GetData<u32>();

    IP = Swap(static_cast<u16>((address & 0xFFFF0000) >> 16));
    CS = Swap(static_cast<u16>(address & 0x0000FFFF));

    LOG(String::ToHex<u16>(CS) + ":" + String::ToHex<u16>(IP));
    break;
  }
  default:
    LOG("[JMP] Don't know what to do with parameter type: " +
        ParameterTypeToString(parameter.GetType()));
    throw UnhandledParameterException(parameter);
  }
}

void CPU::JA(const Instruction& instruction)
{
  if (!CF && !ZF)
    JMP(instruction);
}

void CPU::JBE(const Instruction& instruction)
{
  if (CF || ZF)
    JMP(instruction);
}

void CPU::JB(const Instruction& instruction)
{
  if (CF)
    JMP(instruction);
}

void CPU::JNB(const Instruction& instruction)
{
  if (!CF)
    JMP(instruction);
}

void CPU::JCXZ(const Instruction& instruction)
{
  if (!CX)
    JMP(instruction);
}

void CPU::JG(const Instruction& instruction)
{
  if (!ZF && (SF == OF))
    JMP(instruction);
}

void CPU::JGE(const Instruction& instruction)
{
  if (SF == OF)
    JMP(instruction);
}

void CPU::JL(const Instruction& instruction)
{
  if (SF != OF)
    JMP(instruction);
}

void CPU::JLE(const Instruction& instruction)
{
  if (ZF || SF != OF)
    JMP(instruction);
}

void CPU::JO(const Instruction& instruction)
{
  if (OF)
    JMP(instruction);
}

void CPU::JNO(const Instruction& instruction)
{
  if (!OF)
    JMP(instruction);
}

void CPU::JPE(const Instruction& instruction)
{
  if (PF)
    JMP(instruction);
}

void CPU::JPO(const Instruction& instruction)
{
  if (!PF)
    JMP(instruction);
}

void CPU::JS(const Instruction& instruction)
{
  if (SF)
    JMP(instruction);
}

void CPU::JNS(const Instruction& instruction)
{
  if (!SF)
    JMP(instruction);
}

void CPU::JZ(const Instruction& instruction)
{
  if (ZF)
    JMP(instruction);
}

void CPU::JNZ(const Instruction& instruction)
{
  if (!ZF)
    JMP(instruction);
}

void CPU::CALL(const Instruction& instruction)
{
  auto& parameter = instruction.GetParameters()[0];

  if (parameter.GetType() ==
      Instruction::Parameter::Type::Literal_LongAddress_Immediate) {
    u32 addr = parameter.GetData<u32>();

    u16 offset = (addr & 0xFFFF0000) >> 8;
    u16 segment = (addr & 0xFFFF);

    SP -= sizeof(u16);
    Memory::Get<u16>(SS, SP) = IP;

    CS = segment;
    IP = offset;

    return;
  }

  if (!parameter.IsWord())
    throw UnsupportedParameterException(instruction, parameter);

  u16 offset = ParameterTo<u16>(parameter, instruction.GetPrefix());

  SP -= sizeof(u16);
  Memory::Get<u16>(SS, SP) = IP;

  IP += offset;
}

void CPU::RET(const Instruction&)
{
  IP = Memory::Get<u16>(SS, SP);

  SP += sizeof(u16);
}
