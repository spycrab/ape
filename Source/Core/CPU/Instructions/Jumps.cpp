// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/CPU/Exception.h"

using namespace Core::CPU;

using Type = Instruction::Type;
using PType = Instruction::Parameter::Type;

void CPU::JMP(const Instruction& instruction)
{
  auto& parameter = instruction.GetParameters()[0];
  switch (parameter.GetType()) {
  case PType::Literal_Offset:
    IP += parameter.GetData<i8>();
    break;
  case PType::Literal_WordOffset:
    IP += parameter.GetData<i16>();
    break;
  case PType::Literal_LongAddress_Immediate: {
    u32 address = parameter.GetData<u32>();

    IP = static_cast<u16>((address & 0xFFFF0000) >> 16);
    DS = static_cast<u16>(address & 0x0000FFFF) >> 8;

    LOG(String::ToHex<u16>(DS) + ":" + String::ToHex<u16>(IP));
    break;
  }
  default:
    LOG("[JMP] Don't know what to do with parameter type: " +
        ParameterTypeToString(parameter.GetType()));
    throw UnhandledParameterException();
  }
}

void CPU::JA(const Instruction& instruction)
{
  if (CF || ZF)
    return;

  JMP(instruction);
}

void CPU::JBE(const Instruction& instruction)
{
  if (!CF && !ZF)
    return;

  JMP(instruction);
}

void CPU::JB(const Instruction& instruction)
{
  if (!CF)
    return;

  JMP(instruction);
}

void CPU::JNB(const Instruction& instruction)
{
  if (CF)
    return;

  JMP(instruction);
}

void CPU::JPE(const Instruction& instruction)
{
  if (!PF)
    return;

  JMP(instruction);
}

void CPU::JS(const Instruction& instruction)
{
  if (!SF)
    return;

  JMP(instruction);
}

void CPU::JNS(const Instruction& instruction)
{
  if (SF)
    return;

  JMP(instruction);
}

void CPU::JZ(const Instruction& instruction)
{
  if (!ZF)
    return;

  JMP(instruction);
}

void CPU::JNZ(const Instruction& instruction)
{
  if (ZF)
    return;

  JMP(instruction);
}

void CPU::CALL(const Instruction& instruction)
{
  auto& parameter = instruction.GetParameters()[0];

  if (!parameter.IsWord())
    throw UnsupportedParameterException();

  u16 offset = ParameterTo<u16>(parameter, instruction.GetPrefix());

  SP -= sizeof(u16);
  m_memory.Get<u16>(SS, SP) = IP;

  IP += offset;
}

void CPU::RET(const Instruction&)
{
  IP = m_memory.Get<u16>(SS, SP);

  SP += sizeof(u16);
}
