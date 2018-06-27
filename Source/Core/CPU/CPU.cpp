// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include <type_traits>

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/CPU/Exception.h"
#include "Core/CPU/Flags.h"
#include "Core/CPU/Instruction.h"

#include "Core/Machine.h"

using Machine = Core::Machine;

CPU::CPU(Core::Machine* machine)
    : m_machine(machine), m_memory(machine->GetMemory())
{
}

bool CPU::HandleRepetition()
{
  if (m_repeat_mode != RepeatMode::None)
    CX--;
  switch (m_repeat_mode) {
  case RepeatMode::Repeat:
    return (CX != 0);
  case RepeatMode::Repeat_Zero:
    return (CX != 0 && ZF);
  case RepeatMode::Repeat_Non_Zero:
    return (CX != 0 && !ZF);
  default:
    return false;
  }
}

void CPU::Tick()
{
  const auto old_ip = IP;

  u8 opcode = m_memory.Get<u8>(CS, IP++);
  Instruction ins(opcode, old_ip);

  if (ins.IsPrefix()) {
    ins = Instruction(ins, m_memory.Get<u8>(CS, IP++), old_ip);
  }
  if (!ins.IsResolved()) {
    u8 mod = m_memory.Get<u8>(CS, IP++);
    u8 length = ins.GetLength(mod);

    std::vector<u8> data;

    for (u32 i = 0; i < length; i++) {
      data.push_back(m_memory.Get<u8>(CS, IP++));
    }

    if (!ins.Resolve(mod, data)) {
      LOG("Failed to resolve " + String::ToHex(opcode) + " with mod " +
          String::ToHex(mod));
      throw UnhandledParameterException();
    }
  }

  if (ins.GetType() == Instruction::Type::Invalid) {
    LOG("Invalid instruction hit: " + String::ToHex(opcode));
    return;
    // throw InvalidInstructionException();
  }

  //  LOG(String::ToHex<u16>(DS) + ":" + String::ToHex<u16>(old_ip) + ": " +
  //    ins.ToString());

  using Type = Instruction::Type;
  using PType = Instruction::Parameter::Type;

  switch (ins.GetType()) {
  case Type::ADC: {
    ADC(ins);
    break;
  }
  case Type::ADD: {
    ADD(ins);
    break;
  }
  case Type::AND:
    AND(ins);
    break;
  case Type::CALL:
    CALL(ins);
    break;
  case Type::RET:
    RET(ins);
    break;
  case Type::CBW:
    // Copy the sign bit into all of AH
    AH = AL & (0b1000'0000) ? 0xFF : 0x00;
    break;
  case Type::CLC:
    CF = false;
    break;
  case Type::CMC:
    CF = !CF;
    break;
  case Type::STC:
    CF = true;
    break;
  case Type::CLD:
    DF = false;
    break;
  case Type::STD:
    DF = true;
    break;
  case Type::CLI:
    IF = false;
    break;
  case Type::STI:
    IF = true;
    break;
  case Type::CMP: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord()) {
      i16 dst_i16 = static_cast<i16>(ParameterTo<u16>(dst, ins.GetPrefix()));

      i32 diff = dst_i16;

      if (src.IsWord()) {
        i16 src16 = static_cast<i16>(ParameterTo<u16>(src, ins.GetPrefix()));

        diff -= src16;

        // LOG("Comparing " + String::ToHex<u16>(dst_i16) + " with " +
        //     String::ToHex<u16>(src16));
      } else {
        i8 src8 = static_cast<i8>(ParameterTo<u8>(src, ins.GetPrefix()));

        diff -= src8;

        // LOG("Comparing " + String::ToHex<u16>(dst_i16) + " with " +
        //     String::ToHex<u8>(src8));
      }

      UpdateSF(static_cast<i16>(diff));
      UpdateZF(static_cast<i16>(diff));
      UpdatePF(static_cast<i16>(diff));
      UpdateOF<i16>(diff);
      UpdateCF<i16>(diff);
    } else {
      i8 dst_i8 = static_cast<i8>(ParameterTo<u8>(dst, ins.GetPrefix()));
      i8 src_i8 = static_cast<i8>(ParameterTo<u8>(src, ins.GetPrefix()));

      i32 diff = dst_i8 - src_i8;

      // LOG("Comparing " + String::ToHex<u8>(dst_i8) + " with " +
      //    String::ToHex<u8>(src_i8));

      UpdateSF(static_cast<i8>(diff));
      UpdateZF(static_cast<i8>(diff));
      UpdatePF(static_cast<i8>(diff));
      UpdateOF<i8>(diff);
      UpdateCF<i8>(diff);
    }

    break;
  }
  case Type::CMPSB: {
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

    break;
  }
  case Type::HLT: {
    LOG("CPU halted, waiting for the next interrupt...");

    // TODO: Actually implement interrupts
    while (true) {
    }

    break;
  }
  case Type::INC:
    INC(ins);
    break;
  case Type::DAA: {
    if ((AL & 0xF) > 9 || AF) {
      bool CF_before = CF;

      UpdateCF<u8>(AL + 6);
      CF |= CF_before;
      AL += 6;
      AF = true;
    } else {
      AF = false;
    }

    UpdateSF(AL);
    UpdateZF(AL);
    UpdatePF(AL);

    break;
  }
  case Type::DEC:
    DEC(ins);
    break;
  case Type::IDIV: // TODO: This is a bad way to stub IDIV...
  case Type::DIV:
    DIV(ins);
    break;
  case Type::INT: {
    auto& parameter = ins.GetParameters()[0];
    CallInterrupt(parameter.GetData<u8>());
    break;
  }
  case Type::JMP:
    JMP(ins);
    break;
  case Type::JA:
    JA(ins);
    break;
  case Type::JB:
    JB(ins);
    break;
  case Type::JBE:
    JBE(ins);
    break;
  case Type::JNB:
    JNB(ins);
    break;
  case Type::JCXZ:
    JCXZ(ins);
    break;
  case Type::JS:
    JS(ins);
    break;
  case Type::JNS:
    JNS(ins);
    break;
  case Type::JPE:
    JPE(ins);
    break;
  case Type::JG:
    JG(ins);
    break;
  case Type::JGE:
    JGE(ins);
    break;
  case Type::JL:
    JL(ins);
    break;
  case Type::JLE:
    JL(ins);
    break;
  case Type::JO:
    JO(ins);
    break;
  case Type::JZ:
    JZ(ins);
    break;
  case Type::JNZ:
    JNZ(ins);
    break;
  case Type::LDS: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException();

    if (!dst.IsWord())
      throw UnsupportedParameterException();

    u32 ptr;

    ptr = *reinterpret_cast<u32*>(&ParameterTo<u16&>(src, ins.GetPrefix()));

    u16 segment = (ptr & 0xFFFF0000) >> 16;
    u16 offset = ptr & 0xFFFF;

    DS = segment;
    ParameterTo<u16&>(dst, ins.GetPrefix()) = offset;

    break;
  }
  case Type::LEA: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException();

    if (!dst.IsWord())
      throw UnsupportedParameterException();

    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());

    dst16 = static_cast<u16>(&src16 - m_memory.GetPtr<u16>(DS, 0));

    break;
  }
  case Type::LES: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException();

    if (!dst.IsWord())
      throw UnsupportedParameterException();

    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());

    u32 address = *reinterpret_cast<u32*>(&src16);

    ES = (address & 0xFFFF0000) >> 16;
    dst16 = address & 0xFFFF;
    break;
  }
  case Type::LODSB:
    do {
      AL = m_memory.Get<u8>(DS, SI);

      SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
    } while (HandleRepetition());
    break;
  case Type::LOOP: {
    // LOG("CX = " + String::ToHex(CX));

    if (CX == 0)
      break;

    CX--;

    auto& parameter = ins.GetParameters()[0];

    switch (parameter.GetType()) {
    case PType::Literal_Offset:
      IP += parameter.GetData<i8>();
      break;
    default:
      LOG("[LOOP] Don't know what to do with parameter type: " +
          ParameterTypeToString(parameter.GetType()));
      throw UnhandledParameterException();
    }
    break;
  }
  case Type::MOV: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException();

    if (dst.IsWord()) {
      u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
      u16 src16 = ParameterTo<u16>(src, ins.GetPrefix());

      // LOG("Moving " + String::ToHex(src16) + " -> " + String::ToHex(dst16));

      dst16 = src16;
    } else {
      u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
      u8 src8 = ParameterTo<u8>(src, ins.GetPrefix());

      // LOG("Moving " + String::ToHex(src8) + " -> " + String::ToHex(dst8));

      dst8 = src8;
    }

    break;
  }
  case Type::MOVSB: {
    do {
      u8& dst = m_memory.Get<u8>(ES, DI);
      u8 src = m_memory.Get<u8>(PrefixToValue(ins.GetPrefix()), SI);

      dst = src;

      DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
      SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
    } while (HandleRepetition());

    break;
  }
  case Type::MOVSW: {
    do {
      u16& dst = m_memory.Get<u16>(ES, DI);
      u16 src = m_memory.Get<u16>(PrefixToValue(ins.GetPrefix()), SI);

      dst = src;

      DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
      SI += (DF ? -1 : 1) * static_cast<int>(sizeof(u16));
    } while (HandleRepetition());
    break;
  }
  case Type::IMUL: // TODO: This is a bad way to stub IMUL...
  case Type::MUL: {
    auto& mul = ins.GetParameters()[0];

    if (mul.IsWord()) {
      i32 result =
          static_cast<i32>(AX) * ParameterTo<u16>(mul, ins.GetPrefix());

      AX = result & 0xFFFF;
      DX = (result & 0xFFFF0000) >> 16;

      OF = CF = (AX & 0xff00) != 0;
    } else {
      AX = static_cast<i16>(AL) * ParameterTo<u8>(mul, ins.GetPrefix());

      OF = CF = (AX & 0xff00) != 0;
    }

    break;
  }
  case Type::NOP:
    LOG("NOP? NOP.");
    break;
  case Type::OR:
    OR(ins);
    break;
  case Type::PUSH: {
    auto& data = ins.GetParameters()[0];

    if (!data.IsWord())
      throw UnsupportedParameterException();

    u16 data16 = ParameterTo<u16>(data, ins.GetPrefix());

    SP -= sizeof(u16);
    m_memory.Get<u16>(SS, SP) = data16;

    break;
  }
  case Type::PUSHF: {
    u16 eflags = static_cast<u16>(CF) | (1 << 1) | (static_cast<u16>(PF) << 2) |
                 (static_cast<u16>(AF) << 4) | (static_cast<u16>(ZF) << 6) |
                 (static_cast<u16>(SF) << 7) | (/*TF*/ 0 << 8) |
                 (static_cast<u16>(IF) << 9) | (static_cast<u16>(DF) << 10) |
                 (static_cast<u16>(OF) << 11) | (1 << 14) | (1 << 15);

    SP -= sizeof(u16);
    m_memory.Get<u16>(SS, SP) = eflags;

    break;
  }
  case Type::POP: {
    auto& dst = ins.GetParameters()[0];

    if (!dst.IsWord())
      throw UnsupportedParameterException();

    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());

    dst16 = m_memory.Get<u16>(SS, SP);
    SP += sizeof(u16);

    break;
  }
  case Type::REPZ:
    m_repeat_mode = RepeatMode::Repeat_Zero;
    return;
  case Type::REPNZ:
    m_repeat_mode = RepeatMode::Repeat_Non_Zero;
    return;
  case Type::ROL:
    ROL(ins);
    break;
  case Type::ROR:
    ROR(ins);
    break;
  case Type::SHL:
    SHL(ins);
    break;
  case Type::SHR:
    SHR(ins);
    break;
  case Type::STOSB:
    m_memory.Get<u8>(ES, DI) = AL;

    DI += (DF ? -1 : 1) * static_cast<int>(sizeof(u8));
    break;
  case Type::SBB:
    SBB(ins);
    break;
  case Type::SUB:
    SUB(ins);
    break;
  case Type::TEST:
    TEST(ins);
    break;
  case Type::XCHG: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException();

    if (dst.IsWord()) {
      u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
      u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());
      std::swap(dst16, src16);
    } else {
      u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
      u8& src8 = ParameterTo<u8&>(src, ins.GetPrefix());
      std::swap(dst8, src8);
    }

    break;
  }
  case Type::XOR:
    XOR(ins);
    break;
  default:
    LOG("Don't know what to do with instruction type: " +
        TypeToString(ins.GetType()));
    throw UnhandledInstructionException();
  }

  m_repeat_mode = RepeatMode::None;
}

void CPU::Start()
{
  while (true) {
    Tick();
  }
}

u16 CPU::PrefixToValue(Instruction::SegmentPrefix prefix)
{
  using Prefix = Instruction::SegmentPrefix;
  switch (prefix) {
  case Prefix::CS:
    return CS;
  case Prefix::ES:
    return ES;
  case Prefix::SS:
    return SS;

  case Prefix::DS:
  default:
    return DS;
  }
}

template <class T>
T CPU::ParameterTo(const Instruction::Parameter& parameter,
                   Instruction::SegmentPrefix prefix)
{
  [[maybe_unused]] u16 seg_val = PrefixToValue(prefix);

  static_assert(std::is_same<T, u8>::value || std::is_same<T, u8&>::value ||
                    std::is_same<T, u16>::value || std::is_same<T, u16&>::value,
                "Bad type provided!");

  using Type = Instruction::Parameter::Type;
  if constexpr (std::is_same<T, u8>::value || std::is_same<T, u8&>::value) {
    if (parameter.IsWord()) {
      LOG("[BYTE] Tried to convert a WORD parameter to BYTE for " +
          parameter.ToString(prefix) + "!");
      throw ParameterLengthMismatchException();
    }

    switch (parameter.GetType()) {
    case Type::AL:
      return AL;
    case Type::AH:
      return AH;

    case Type::BL:
      return BL;
    case Type::BH:
      return BH;

    case Type::CL:
      return CL;
    case Type::CH:
      return CH;

    case Type::DL:
      return DL;
    case Type::DH:
      return DH;

    case Type::Value_WordAddress:
      return m_memory.Get<u8>(seg_val, parameter.GetData<u16>());
    case Type::Value_BP_Offset:
      return m_memory.Get<u8>(seg_val, BP + parameter.GetData<u8>());
    case Type::Value_BP_WordOffset:
      return m_memory.Get<u8>(seg_val, BP + parameter.GetData<u16>());
    case Type::Value_BP_DI:
      return m_memory.Get<u8>(seg_val, BP + DI);
    case Type::Value_BP_DI_Offset:
      return m_memory.Get<u8>(seg_val, BP + DI + parameter.GetData<u8>());
    case Type::Value_BP_DI_WordOffset:
      return m_memory.Get<u8>(seg_val, BP + DI + parameter.GetData<u16>());
    case Type::Value_BP_SI:
      return m_memory.Get<u8>(seg_val, BP + SI);
    case Type::Value_BP_SI_Offset:
      return m_memory.Get<u8>(seg_val, BP + SI + parameter.GetData<u8>());
    case Type::Value_BP_SI_WordOffset:
      return m_memory.Get<u8>(seg_val, BP + SI + parameter.GetData<u16>());
    case Type::Value_BX:
      return m_memory.Get<u8>(seg_val, BX);
    case Type::Value_BX_Offset:
      return m_memory.Get<u8>(seg_val, BX + parameter.GetData<u8>());
    case Type::Value_BX_WordOffset:
      return m_memory.Get<u8>(seg_val, BX + parameter.GetData<u16>());
    case Type::Value_BX_SI:
      return m_memory.Get<u8>(seg_val, BX + SI);
    case Type::Value_BX_SI_Offset:
      return m_memory.Get<u8>(seg_val, BX + SI + parameter.GetData<u8>());
    case Type::Value_BX_SI_WordOffset:
      return m_memory.Get<u8>(seg_val, BX + SI + parameter.GetData<u16>());
    case Type::Value_BX_DI:
      return m_memory.Get<u8>(seg_val, BX + DI);
    case Type::Value_BX_DI_Offset:
      return m_memory.Get<u8>(seg_val, BX + DI + parameter.GetData<u8>());
    case Type::Value_BX_DI_WordOffset:
      return m_memory.Get<u8>(seg_val, BX + DI + parameter.GetData<u16>());
    case Type::Value_DI:
      return m_memory.Get<u8>(seg_val, DI);
    case Type::Value_DI_Offset:
      return m_memory.Get<u8>(seg_val, DI + parameter.GetData<u8>());
    case Type::Value_DI_WordOffset:
      return m_memory.Get<u8>(seg_val, DI + parameter.GetData<u16>());
    case Type::Value_SI:
      return m_memory.Get<u8>(seg_val, SI);
    case Type::Value_SI_Offset:
      return m_memory.Get<u8>(seg_val, SI + parameter.GetData<u8>());
    case Type::Value_SI_WordOffset:
      return m_memory.Get<u8>(seg_val, SI + parameter.GetData<u16>());
    default:
      if constexpr (std::is_same<T, u8>::value) {
        switch (parameter.GetType()) {
        case Type::Implied_0:
          return 0;
        case Type::Implied_1:
          return 1;
        case Type::Implied_3:
          return 3;
        case Type::Literal_Byte:
        case Type::Literal_Byte_Immediate:
          return parameter.GetData<u8>();
        default:
          break;
        }
      }

      LOG("[BYTE] Unknown type: " + ParameterTypeToString(parameter.GetType()));
      throw UnhandledParameterException();
    }
  } else if constexpr (std::is_same<T, u16>::value ||
                       std::is_same<T, u16&>::value) {
    if (!parameter.IsWord())
      throw ParameterLengthMismatchException();

    switch (parameter.GetType()) {
    case Type::AX:
      return AX;
    case Type::BX:
      return BX;
    case Type::CX:
      return CX;
    case Type::DX:
      return DX;

    case Type::CS:
      return CS;
    case Type::DS:
      return DS;
    case Type::ES:
      return ES;
    case Type::SS:
      return SS;

    case Type::IP:
      return IP;
    case Type::BP:
      return BP;
    case Type::SP:
      return SP;
    case Type::SI:
      return SI;
    case Type::DI:
      return DI;

    case Type::Value_WordAddress_Word:
      return m_memory.Get<u16>(seg_val, parameter.GetData<u16>());
    case Type::Value_DI_Word:
      return m_memory.Get<u16>(seg_val, DI);
    case Type::Value_DI_Offset_Word:
      return m_memory.Get<u16>(seg_val, DI + parameter.GetData<u8>());
    case Type::Value_SI_Word:
      return m_memory.Get<u16>(seg_val, SI);
    case Type::Value_SI_Offset_Word:
      return m_memory.Get<u16>(seg_val, SI + parameter.GetData<u8>());
    case Type::Value_BP_Offset_Word:
      return m_memory.Get<u16>(seg_val, BP + parameter.GetData<u8>());
    case Type::Value_BP_WordOffset_Word:
      return m_memory.Get<u16>(seg_val, BP + parameter.GetData<u16>());
    case Type::Value_BP_DI_Word:
      return m_memory.Get<u16>(seg_val, BP + DI);
    case Type::Value_BP_DI_Offset_Word:
      return m_memory.Get<u16>(seg_val, BP + DI + parameter.GetData<u8>());
    case Type::Value_BP_DI_WordOffset_Word:
      return m_memory.Get<u16>(seg_val, BP + DI + parameter.GetData<u16>());
    case Type::Value_BP_SI_Word:
      return m_memory.Get<u16>(seg_val, BP + SI);
    case Type::Value_BP_SI_Offset_Word:
      return m_memory.Get<u16>(seg_val, BP + SI + parameter.GetData<u8>());
    case Type::Value_BP_SI_WordOffset_Word:
      return m_memory.Get<u16>(seg_val, BP + SI + parameter.GetData<u16>());
    case Type::Value_BX_Word:
      return m_memory.Get<u16>(seg_val, BX);
    case Type::Value_BX_Offset_Word:
      return m_memory.Get<u16>(seg_val, BX + parameter.GetData<u8>());
    case Type::Value_BX_DI_Word:
      return m_memory.Get<u16>(seg_val, BX + DI);
    case Type::Value_BX_DI_Offset_Word:
      return m_memory.Get<u16>(seg_val, BX + DI + parameter.GetData<u8>());
    case Type::Value_BX_DI_WordOffset_Word:
      return m_memory.Get<u16>(seg_val, BX + DI + parameter.GetData<u16>());
    case Type::Value_BX_SI_Word:
      return m_memory.Get<u16>(seg_val, BX + SI);
    case Type::Value_BX_SI_Offset_Word:
      return m_memory.Get<u16>(seg_val, BX + SI + parameter.GetData<u8>());
    case Type::Value_BX_SI_WordOffset_Word:
      return m_memory.Get<u16>(seg_val, BX + SI + parameter.GetData<u16>());

    default:
      if constexpr (std::is_same<T, u16>::value) {
        switch (parameter.GetType()) {
        case Type::Literal_Word:
          return parameter.GetData<u16>();
        case Type::Literal_Word_Immediate:
          return parameter.GetData<u16>();
        case Type::Literal_WordOffset:
          return parameter.GetData<u16>();
        default:
          LOG("[WORD] Unknown type: " +
              ParameterTypeToString(parameter.GetType()));
          throw UnhandledParameterException();
          break;
        }
      } else {
        LOG("[WORD] Unknown type: " +
            ParameterTypeToString(parameter.GetType()));
        throw UnhandledParameterException();
      }
    }
  }
}
