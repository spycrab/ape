// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Instruction.h"

#include <algorithm>
#include <array>
#include <iostream>

#include "Common/Logger.h"
#include "Common/String.h"
#include "Core/CPU/Exception.h"

using Core::CPU::Instruction;

static std::string PrefixToString(Instruction::SegmentPrefix prefix)
{
  using Prefix = Instruction::SegmentPrefix;
  switch (prefix) {
  case Prefix::CS:
    return "CS:";
  case Prefix::DS:
    return "DS:";
  case Prefix::ES:
    return "ES:";
  case Prefix::SS:
    return "SS:";
  default:
    return "";
  }
}

std::string
Core::CPU::ParameterTypeToString(const Instruction::Parameter::Type& type,
                                 Instruction::SegmentPrefix prefix)
{
  using Type = Instruction::Parameter::Type;
  switch (type) {
  case Type::AX:
    return "AX";
  case Type::AL:
    return "AL";
  case Type::AH:
    return "AH";
  case Type::BX:
    return "BX";
  case Type::BL:
    return "BL";
  case Type::BH:
    return "BH";
  case Type::CX:
    return "CX";
  case Type::CL:
    return "CL";
  case Type::CH:
    return "CH";
  case Type::DX:
    return "DX";
  case Type::DL:
    return "DL";
  case Type::DH:
    return "DH";

  case Type::CS:
    return "CS";
  case Type::DS:
    return "DS";
  case Type::ES:
    return "ES";
  case Type::SS:
    return "SS";

  case Type::IP:
    return "IP";
  case Type::DI:
    return "DI";
  case Type::SI:
    return "SI";
  case Type::BP:
    return "BP";
  case Type::SP:
    return "SP";

  case Type::Implied_0:
    return "0";
  case Type::Implied_1:
    return "1";
  case Type::Implied_3:
    return "3";

  case Type::Modifier_Register_Byte:
    return "RegByte";
  case Type::Modifier_Register_Word:
    return "RegWord";
  case Type::Modifier_Register_Segment:
    return "RegSegment";
  case Type::Modifier_Any_Byte:
    return "AnyByte";
  case Type::Modifier_Any_Word:
    return "AnyWord";

  case Type::Literal_Byte:
    return "LiteralByte";
  case Type::Literal_Byte_Immediate:
    return "LiteralByte[i]";
  case Type::Literal_Word:
    return "LiteralWord";
  case Type::Literal_Word_Immediate:
    return "LiteralWord[i]";
  case Type::Literal_Offset:
    return "LiteralOffset";
  case Type::Literal_WordOffset:
    return "LiteralWordOffset";
  case Type::Literal_LongAddress_Immediate:
    return "LiteralLongAddress[i]";

  case Type::Value_BP_SI:
    return "[" + PrefixToString(prefix) + "BP+SI]";
  case Type::Value_BP_SI_Word:
    return "word [" + PrefixToString(prefix) + "BP+SI]";
  case Type::Value_BP_SI_Offset:
    return "[" + PrefixToString(prefix) + "BP+SI+offset]";
  case Type::Value_BP_SI_Offset_Word:
    return "word [" + PrefixToString(prefix) + "BP+SI+offset]";
  case Type::Value_BP_DI_Offset:
    return "[" + PrefixToString(prefix) + "BP+DI+offset]";
  case Type::Value_BP_DI_Offset_Word:
    return "word [" + PrefixToString(prefix) + "BP+DI+offset]";
  case Type::Value_BP_WordOffset:
    return "[" + PrefixToString(prefix) + "BP+wOffset]";
  case Type::Value_BP_WordOffset_Word:
    return "word [" + PrefixToString(prefix) + "BP+wOffset]";
  case Type::Value_BP_Offset:
    return "[" + PrefixToString(prefix) + "BP+bOffset]";
  case Type::Value_BP_Offset_Word:
    return "word [" + PrefixToString(prefix) + "BP+bOffset]";
  case Type::Value_BX_SI:
    return "[" + PrefixToString(prefix) + "BX+SI]";
  case Type::Value_BX_SI_Word:
    return "word [" + PrefixToString(prefix) + "BX+SI]";
  case Type::Value_BX_DI:
    return "[" + PrefixToString(prefix) + "BX+SI]";
  case Type::Value_BX_DI_Word:
    return "word [" + PrefixToString(prefix) + "BX+SI]";
  case Type::Value_BX_SI_Offset:
    return "[" + PrefixToString(prefix) + "BX+SI+offset]";
  case Type::Value_BX_SI_Offset_Word:
    return "word [" + PrefixToString(prefix) + "BX+SI+offset]";
  case Type::Value_BX_DI_Offset:
    return "[" + PrefixToString(prefix) + "BX+DI+offset]";
  case Type::Value_BX_DI_Offset_Word:
    return "word [" + PrefixToString(prefix) + "BX+DI+offset]";
  case Type::Value_BX:
    return "[" + PrefixToString(prefix) + "BX]";
  case Type::Value_BX_Word:
    return "word [" + PrefixToString(prefix) + "BX]";
  case Type::Value_BX_Offset:
    return "[" + PrefixToString(prefix) + "BX+offset]";
  case Type::Value_BX_Offset_Word:
    return "word [" + PrefixToString(prefix) + "BX+offset]";
  case Type::Value_SI:
    return "[" + PrefixToString(prefix) + "SI]";
  case Type::Value_SI_Word:
    return "word [" + PrefixToString(prefix) + "SI]";
  case Type::Value_DI:
    return "[" + PrefixToString(prefix) + "DI]";
  case Type::Value_DI_Word:
    return "word [" + PrefixToString(prefix) + "SI]";
  case Type::Value_SI_Offset:
    return "[" + PrefixToString(prefix) + "SI+offset]";
  case Type::Value_SI_Offset_Word:
    return "word [" + PrefixToString(prefix) + "SI+offset]";
  case Type::Value_DI_Offset:
    return "[" + PrefixToString(prefix) + "DI+offset]";
  case Type::Value_DI_Offset_Word:
    return "word [" + PrefixToString(prefix) + "DI+offset]";
  case Type::Value_WordAddress:
    return "[wordAddr]";
  case Type::Value_WordAddress_Word:
    return "word [wordAddr]";
  case Type::None:
    return "None";
  }

  return "???";
}

std::string Core::CPU::TypeToString(const Instruction::Type& type)
{
  using Type = Instruction::Type;
  switch (type) {
  case Type::JMP:
    return "JMP";
  case Type::JZ:
    return "JZ";
  case Type::JNZ:
    return "JNZ";
  case Type::JC:
    return "JC";
  case Type::JNC:
    return "JNC";
  case Type::JCXZ:
    return "JCXZ";
  case Type::JE:
    return "JE";
  case Type::JPE:
    return "JPE";
  case Type::JNS:
    return "JNS";
  case Type::JNO:
    return "JNO";
  case Type::JNE:
    return "JNE";
  case Type::JL:
    return "JL";
  case Type::JLE:
    return "JLE";
  case Type::JGE:
    return "JGE";
  case Type::JG:
    return "JG";
  case Type::JA:
    return "JA";
  case Type::JB:
    return "JB";
  case Type::JNB:
    return "JNB";
  case Type::JPO:
    return "JPO";
  case Type::JO:
    return "JO";
  case Type::JS:
    return "JS";
  case Type::JBE:
    return "JBE";

  case Type::CALL:
    return "CALL";
  case Type::RET:
    return "RET";
  case Type::IRET:
    return "IRET";
  case Type::RETF:
    return "RETF";
  case Type::REPZ:
    return "REPZ";
  case Type::REPNZ:
    return "REPNZ";
  case Type::HLT:
    return "HLT";
  case Type::LOOP:
    return "LOOP";
  case Type::LOOPZ:
    return "LOOPZ";
  case Type::LOOPNZ:
    return "LOOPNZ";

  case Type::CLC:
    return "CLC";
  case Type::STC:
    return "STC";
  case Type::CLI:
    return "CLI";
  case Type::STI:
    return "STI";
  case Type::CLD:
    return "CLD";
  case Type::STD:
    return "STD";

  case Type::MOV:
    return "MOV";
  case Type::LEA:
    return "LEA";
  case Type::LDS:
    return "LDS";
  case Type::LES:
    return "LES";
  case Type::XCHG:
    return "XCHG";

  case Type::PUSH:
    return "PUSH";
  case Type::PUSHF:
    return "PUSHF";
  case Type::POP:
    return "POP";
  case Type::POPF:
    return "POPF";

  case Type::LODSB:
    return "LODSB";
  case Type::LODSW:
    return "LODSW";
  case Type::SCASB:
    return "SCASB";
  case Type::SCASW:
    return "SCASW";
  case Type::STOSB:
    return "STOSB";
  case Type::STOSW:
    return "STOSW";
  case Type::CMPSB:
    return "CMPSB";
  case Type::CMPSW:
    return "CMPSW";
  case Type::MOVSB:
    return "MOVSB";
  case Type::MOVSW:
    return "MOVSW";

  case Type::CMP:
    return "CMP";
  case Type::INC:
    return "INC";
  case Type::DEC:
    return "DEC";
  case Type::ADD:
    return "ADD";
  case Type::SUB:
    return "SUB";
  case Type::ADC:
    return "ADC";
  case Type::SBC:
    return "SBC";
  case Type::DIV:
    return "DIV";
  case Type::MUL:
    return "MUL";
  case Type::SBB:
    return "SBB";

  case Type::OR:
    return "OR";
  case Type::XOR:
    return "XOR";
  case Type::AND:
    return "AND";
  case Type::NEG:
    return "NEG";
  case Type::SHL:
    return "SHL";

  case Type::IN:
    return "IN";
  case Type::OUT:
    return "OUT";
  case Type::LOCK:
    return "LOCK";
  case Type::XLAT:
    return "XLAT";
  case Type::INT:
    return "INT";
  case Type::INTO:
    return "INTO";
  case Type::CMC:
    return "CMC";
  case Type::AAA:
    return "AAA";
  case Type::AAD:
    return "AAD";
  case Type::AAM:
    return "AAM";
  case Type::AAS:
    return "AAS";
  case Type::DAA:
    return "DAA";
  case Type::DAS:
    return "DAS";
  case Type::TEST:
    return "TEST";
  case Type::LAHF:
    return "LAHF";
  case Type::SAHF:
    return "SAHF";
  case Type::NOP:
    return "NOP";
  case Type::CWD:
    return "CWD";
  case Type::CBW:
    return "CBW";
  case Type::WAIT:
    return "WAIT";

  case Type::ROL:
    return "ROL";

  case Type::PREFIX_DS:
    return "DS:";
  case Type::PREFIX_SS:
    return "SS:";
  case Type::PREFIX_CS:
    return "CS:";
  case Type::PREFIX_ES:
    return "ES:";

  case Type::GRP1:
    return "GRP1";
  case Type::GRP2:
    return "GRP2";
  case Type::GRP3a:
    return "GRP3a";
  case Type::GRP3b:
    return "GRP3b";
  case Type::GRP4:
    return "GRP4";
  case Type::GRP5:
    return "GRP5";

  case Type::Invalid:
    return "INVALID";
  }

  return "???";
}

std::string Instruction::Parameter::ToString(SegmentPrefix prefix,
                                             u32 offset) const
{
  if (Core::CPU::ParameterNeedsResolving(m_type)) {
    const u16 data16 = static_cast<u16>(m_data);
    const u8 data8 = static_cast<u8>(m_data);

    using Type = Parameter::Type;
    switch (m_type) {
    case Type::Literal_Byte:
    case Type::Literal_Byte_Immediate:
      return String::ToHex(data8);
    case Type::Literal_Word:
    case Type::Literal_Word_Immediate:
      return String::ToHex(data16);
    case Type::Literal_Offset:
      return String::ToHex<u8>(
          static_cast<u8>(offset + 2 + static_cast<i8>(data16)));
    case Type::Literal_WordOffset:
      return String::ToHex<u16>(
          static_cast<u16>(offset + 3 + static_cast<i16>(data16)));
    case Type::Value_WordAddress_Word:
      return "word [" + String::ToHex(data16) + "]";
    case Type::Value_WordAddress:
      return "[" + String::ToHex(data16) + "]";
    case Type::Value_BP_Offset:
      return "[" + PrefixToString(prefix) + "BP+" + String::ToHex(data16) + "]";
    case Type::Value_BP_WordOffset:
      return "[" + PrefixToString(prefix) + "BP+" + String::ToHex(data16) + "]";
    case Type::Value_BX_DI_Offset:
      return "[" + PrefixToString(prefix) + "BX+DI+" + String::ToHex(data8) +
             "]";
    case Type::Value_BX_SI_Offset:
      return "[" + PrefixToString(prefix) + "BX+SI+" + String::ToHex(data8) +
             "]";
    case Type::Value_BP_SI_Offset:
      return "[" + PrefixToString(prefix) + "BP+SI+" + String::ToHex(data8) +
             "]";
    case Type::Value_BP_DI_Offset:
      return "[" + PrefixToString(prefix) + "BP+DI+" + String::ToHex(data8) +
             "]";
    case Type::Value_BX_Offset:
      return "[" + PrefixToString(prefix) + "BX+" + String::ToHex(data8) + "]";
    case Type::Value_BX_Offset_Word:
      return "word [" + PrefixToString(prefix) + "BX+" + String::ToHex(data8) +
             "]";
    case Type::Value_SI_Offset_Word:
      return "word [SI+" + String::ToHex(data16) + "]";
    case Type::Value_SI_Offset:
      return "[" + PrefixToString(prefix) + "SI+" + String::ToHex(data8) + "]";
    case Type::Value_DI_Offset:
      return "[" + PrefixToString(prefix) + "DI+" + String::ToHex(data8) + "]";
    case Type::Literal_LongAddress_Immediate:
      return String::ToHex<u32>(m_data);
    default:
      return String::ToHex<u32>(m_data);
    }
  } else {
    return ParameterTypeToString(m_type, prefix);
  }
}

bool Core::CPU::ParameterNeedsResolving(
    const Instruction::Parameter::Type& type)
{
  using Type = Instruction::Parameter::Type;
  constexpr std::array resolved_params{Type::AL,
                                       Type::AH,
                                       Type::BL,
                                       Type::BH,
                                       Type::CL,
                                       Type::CH,
                                       Type::DL,
                                       Type::DH,
                                       Type::AX,
                                       Type::BX,
                                       Type::CX,
                                       Type::DX,
                                       Type::CS,
                                       Type::DS,
                                       Type::ES,
                                       Type::SS,
                                       Type::IP,
                                       Type::BP,
                                       Type::SP,
                                       Type::DI,
                                       Type::SI,
                                       Type::Implied_0,
                                       Type::Implied_1,
                                       Type::Implied_3,
                                       Type::Value_BP_SI,
                                       Type::Value_BP_DI,
                                       Type::Value_BX_SI_Word,
                                       Type::Value_BX_SI,
                                       Type::Value_BX_DI,
                                       Type::Value_BX,
                                       Type::Value_BX_Word,
                                       Type::Value_DI,
                                       Type::Value_SI_Word,
                                       Type::None};

  return std::find(resolved_params.begin(), resolved_params.end(), type) ==
         resolved_params.end();
}

bool Instruction::IsResolved()
{
  auto type = GetType();
  if (type == Type::GRP1 || type == Type::GRP2 || type == Type::GRP3a ||
      type == Type::GRP3b || type == Type::GRP4 || type == Type::GRP5)
    return false;

  for (const auto& param : m_parameters) {
    if (!param.IsResolved())
      return false;
  }

  return true;
}

bool Instruction::IsPrefix() const
{
  return m_type == Type::PREFIX_CS || m_type == Type::PREFIX_DS ||
         m_type == Type::PREFIX_ES || m_type == Type::PREFIX_SS;
}

static bool ResolveRM8(Instruction::Parameter& param, u8 rm_bits, u8 modrm)
{
  using PType = Instruction::Parameter::Type;
  switch (rm_bits) {
  case 0b000: // AL
    param.Resolve(PType::AL);
    break;
  case 0b001: // CL
    param.Resolve(PType::CL);
    break;
  case 0b010: // DL
    param.Resolve(PType::DL);
    break;
  case 0b011: // BL
    param.Resolve(PType::BL);
    break;
  case 0b100: // AH
    param.Resolve(PType::AH);
    break;
  case 0b101: // CH
    param.Resolve(PType::CH);
    break;
  case 0b110: // DH
    param.Resolve(PType::DH);
    break;
  case 0b111: // BH
    param.Resolve(PType::BH);
    break;
  default:
    std::cerr << "Don't know how to resolve the RB modifier "
              << String::ToBin(rm_bits) << ": " << String::ToHex(modrm)
              << std::endl;
    return false;
  }
  return true;
}

static bool ResolveRM16(Instruction::Parameter& param, u8 rm_bits, u8 modrm)
{
  using PType = Instruction::Parameter::Type;
  switch (rm_bits) {
  case 0b000: // AX
    param.Resolve(PType::AX);
    break;
  case 0b001: // CX
    param.Resolve(PType::CX);
    break;
  case 0b010: // DX
    param.Resolve(PType::DX);
    break;
  case 0b011: // BX
    param.Resolve(PType::BX);
    break;
  case 0b100: // SP
    param.Resolve(PType::SP);
    break;
  case 0b101: // BP
    param.Resolve(PType::BP);
    break;
  case 0b110: // SI
    param.Resolve(PType::SI);
    break;
  case 0b111: // DI
    param.Resolve(PType::DI);
    break;
  default:
    std::cerr << "Don't know how to resolve the RW modifier "
              << String::ToBin(rm_bits) << ": " << String::ToHex(modrm)
              << std::endl;
    return false;
  }
  return true;
}

bool Instruction::Resolve(u8 modrm, std::vector<u8> data)
{

  // MOD RM Layout
  // |7 6 |5 4 3|2 1 0|
  // |----------------|
  // |Mod | Reg | R/M |

  u8 mod_bits = (modrm & 0xC0) >> 3;
  u8 reg_bits = (modrm & 0x38) >> 3;
  u8 rm_bits = modrm & 0x07;
  u8 mod_cmb = mod_bits | rm_bits;

  if (m_type == Type::GRP1) {
    switch (reg_bits) {
    case 0x02:
      m_type = Type::ADC;
      break;
    case 0x07:
      m_type = Type::CMP;
      break;
    default:
      std::cerr << "(GRP1) Don't know what to do with "
                << String::ToHex(reg_bits) << std::endl;
      return false;
    }
  }

  if (m_type == Type::GRP2) {

    switch (reg_bits) {
    case 0x00:
      m_type = Type::ROL;
      break;
    case 0x04:
      m_type = Type::SHL;
      break;
    default:
      std::cerr << "(GRP2) Don't know what to do with "
                << String::ToHex(reg_bits) << std::endl;
      return false;
    }
  }

  if (m_type == Type::GRP3a) {
    switch (reg_bits) {
    default:
      std::cerr << "(GRP3a) Don't know what to do with "
                << String::ToHex(reg_bits) << std::endl;
      return false;
    }
  }

  if (GetType() == Type::GRP3b) {
    switch (reg_bits) {
    case 0x06:
      m_type = Type::DIV;
      break;
    case 0x04:
      m_type = Type::MUL;
      break;
    default:
      std::cerr << "(GRP3b) Don't know what to do with "
                << String::ToHex(reg_bits) << std::endl;
      return false;
    }
  }

  if (GetType() == Type::GRP4) {
    switch (reg_bits) {
    case 0x00:
      m_type = Type::INC;
      break;
    default:
      std::cerr << "(GRP4) Don't know what to do with "
                << String::ToHex(reg_bits) << std::endl;
      return false;
    }
  }

  if (GetType() == Type::GRP5) {
    switch (reg_bits) {
    default:
      std::cerr << "(GRP5) Don't know what to do with "
                << String::ToHex(reg_bits) << std::endl;
      return false;
    }
  }

  if (GetType() == Type::Invalid)
    return false;

  for (auto& param : m_parameters) {
    if (!param.IsResolved()) {
      using PType = Parameter::Type;
      switch (param.GetType()) {
      case PType::Modifier_Register_Byte:
        if (!ResolveRM8(param, reg_bits, modrm))
          return false;
        break;
      case PType::Modifier_Register_Word:
        if (!ResolveRM16(param, reg_bits, modrm))
          return false;
        break;
      case PType::Modifier_Register_Segment:
        switch (mod_cmb) {
        case 0x18: // SS
          param.Resolve(PType::SS);
          break;
        default:
          std::cerr << "Don't know how to resolve the RS modifier "
                    << String::ToBin(mod_cmb) << "(" << String::ToHex(mod_cmb)
                    << ")" << std::endl;
          return false;
        }
        break;
      case PType::Modifier_Any_Byte:
        if (mod_bits == 0b11'000) {
          if (!ResolveRM8(param, rm_bits, modrm))
            return false;
          break;
        }
        switch (mod_cmb) {
        case 0b00'000: // [BX+SI]
          param.Resolve(PType::Value_BX_SI);
          break;
        case 0b00'001: // [BX+DI]
          param.Resolve(PType::Value_BX_DI);
          break;
        case 0b00'010: // [BP+SI]
          param.Resolve(PType::Value_BP_SI);
          break;
        case 0b00'011: // [BP+DI]
          param.Resolve(PType::Value_BP_DI);
          break;
        case 0b00'100: // [SI]
          param.Resolve(PType::Value_SI);
          break;
        case 0b00'101: // [DI]
          param.Resolve(PType::Value_DI);
          break;
        case 0b00'110: // [Addr16]
          param.Resolve(PType::Value_WordAddress,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b00'111: // [BX]
          param.Resolve(PType::Value_BX);

        case 0b01'000: // [BX+SI+offset]
          param.Resolve(PType::Value_BX_SI_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'001: // [BX + DI + offset]
          param.Resolve(PType::Value_BX_DI_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'010: // [BP+SI+offset]
          param.Resolve(PType::Value_BP_SI_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'011: // [BP+DI+offset]
          param.Resolve(PType::Value_BP_DI_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'100: // [SI+offset]
          param.Resolve(PType::Value_SI_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'101: // [DI+offset]
          param.Resolve(PType::Value_DI_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'110: // [BP+offset]
          param.Resolve(PType::Value_BP_Offset,
                        *reinterpret_cast<u8*>(data.data()));
        case 0b01'111: // [BX+offset]
          param.Resolve(PType::Value_BX_Offset,
                        *reinterpret_cast<u8*>(data.data()));
          break;

        case 0b10'000: // [BX+SI+wOffset]
          param.Resolve(PType::Value_BX_SI_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'001: // [BX+DI+wOffset]
          param.Resolve(PType::Value_BX_DI_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'010: // [BP+SI+wOffset]
          param.Resolve(PType::Value_BP_SI_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'011: // [BP+DI+wOffset]
          param.Resolve(PType::Value_BP_DI_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'100: // [SI+wOffset]
          param.Resolve(PType::Value_SI_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'101: // [DI+wOffset]
          param.Resolve(PType::Value_DI_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'110: // [BP+wOffset]
          param.Resolve(PType::Value_BP_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
        case 0b10'111: // [BX+wOffset]
          param.Resolve(PType::Value_BX_WordOffset,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        default:
          std::cerr << "Don't know how to resolve the AB modifier "
                    << String::ToBin(mod_cmb) << "(" << String::ToHex(mod_cmb)
                    << "): " << String::ToHex(modrm) << std::endl;
          return false;
        }
        break;
      case PType::Modifier_Any_Word:
        if (mod_bits == 0b11'000) {
          if (!ResolveRM16(param, rm_bits, modrm))
            return false;
          break;
        }
        switch (mod_cmb) {
        case 0b00'000: // word [BX+SI]
          param.Resolve(PType::Value_BX_SI_Word);
          break;
        case 0b00'001: // [BX+DI]
          param.Resolve(PType::Value_BX_DI_Word);
          break;
        case 0b00'010: // [BP+SI]
          param.Resolve(PType::Value_BP_SI_Word);
          break;
        case 0b00'011: // [BP+DI]
          param.Resolve(PType::Value_BP_DI_Word);
          break;
        case 0b00'100: // [SI]
          param.Resolve(PType::Value_SI_Word);
          break;
        case 0b00'101: // [DI]
          param.Resolve(PType::Value_DI_Word);
          break;
        case 0b00'110: // [Addr16]
          param.Resolve(PType::Value_WordAddress_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b00'111: // [BX]
          param.Resolve(PType::Value_BX_Word);
          break;
        case 0b01'000: // [BX+SI+offset]
          param.Resolve(PType::Value_BX_SI_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'001: // [BX + DI + offset]
          param.Resolve(PType::Value_BX_DI_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'010: // [BP+SI+offset]
          param.Resolve(PType::Value_BP_SI_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'011: // [BP+DI+offset]
          param.Resolve(PType::Value_BP_DI_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'100: // [SI+offset]
          param.Resolve(PType::Value_SI_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'101: // [DI+offset]
          param.Resolve(PType::Value_DI_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;
        case 0b01'110: // [BP+offset]
          param.Resolve(PType::Value_BP_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
        case 0b01'111: // [BX+offset]
          param.Resolve(PType::Value_BX_Offset_Word,
                        *reinterpret_cast<u8*>(data.data()));
          break;

        case 0b10'000: // [BX+SI+wOffset]
          param.Resolve(PType::Value_BX_SI_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'001: // [BX+DI+wOffset]
          param.Resolve(PType::Value_BX_DI_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'010: // [BP+SI+wOffset]
          param.Resolve(PType::Value_BP_SI_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'011: // [BP+DI+wOffset]
          param.Resolve(PType::Value_BP_DI_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'100: // [SI+wOffset]
          param.Resolve(PType::Value_SI_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'101: // [DI+wOffset]
          param.Resolve(PType::Value_DI_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;
        case 0b10'110: // [BP+wOffset]
          param.Resolve(PType::Value_BP_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
        case 0b10'111: // [BX+wOffset]
          param.Resolve(PType::Value_BX_WordOffset_Word,
                        *reinterpret_cast<u16*>(data.data()));
          break;

        default:
          std::cerr << "Don't know how to resolve the AW modifier "
                    << String::ToBin(mod_cmb) << "(" << String::ToHex(mod_cmb)
                    << "): " << String::ToHex(modrm) << std::endl;
          return false;
        }
        break;
      case PType::Value_BP_WordOffset:
        param.Resolve(*reinterpret_cast<u8*>(data.data()) | (modrm << 8));
        break;
      case PType::Literal_Offset:
        param.Resolve(modrm);
        break;
      case PType::Literal_Word_Immediate:
        param.Resolve(*reinterpret_cast<u8*>(data.data()) << 8 | modrm);
        break;
      case PType::Literal_Byte_Immediate:
        param.Resolve(modrm);
        break;
      case PType::Literal_Word:
        param.Resolve(*reinterpret_cast<u16*>(data.data()));
        break;
      case PType::Literal_WordOffset:
        param.Resolve(*reinterpret_cast<u8*>(data.data()) << 8 | modrm);
        break;
      case PType::Literal_Byte:
        param.Resolve(*reinterpret_cast<u16*>(data.data()));
        break;
      case PType::Value_WordAddress:
      case PType::Value_WordAddress_Word:
        param.Resolve(*reinterpret_cast<u8*>(data.data()) << 8 | modrm);
        break;
      case PType::Literal_LongAddress_Immediate:
        param.Resolve(modrm << 24 | data[0] << 16 | data[1] << 8 | data[2]);
        break;
      default:
        std::cerr << "Don't know how to resolve "
                  << Core::CPU::ParameterTypeToString(param.GetType(), m_prefix)
                  << std::endl;
        return false;
      }
    }
  }

  return true;
}

Instruction::Parameter::Parameter(Parameter::Type type) : m_type(type)
{
  this->m_resolved = !ParameterNeedsResolving(type);
}

void Instruction::Parameter::Resolve(u32 data)
{
  if (this->m_resolved)
    return;

  this->m_data = data;
  this->m_resolved = true;
}

void Instruction::Parameter::Resolve(Parameter::Type type, u32 data)
{
  if (this->m_resolved)
    return;

  this->m_type = type;
  this->m_data = data;
  this->m_resolved = true;
}

Instruction::Parameter::Type Instruction::Parameter::GetType() const
{
  return m_type;
}
bool Instruction::Parameter::IsResolved() const { return m_resolved; }

bool Instruction::Parameter::IsWord() const
{
  auto t = m_type;
  return (t == Type::AX || t == Type::BX || t == Type::CX || t == Type::DX ||
          t == Type::CS || t == Type::DS || t == Type::ES || t == Type::SS ||
          t == Type::IP || t == Type::BP || t == Type::SP || t == Type::DI ||
          t == Type::SI || t == Type::Literal_Word ||
          t == Type::Literal_Word_Immediate || t == Type::Literal_WordOffset ||
          t == Type::Modifier_Register_Word ||
          t == Type::Modifier_Register_Segment ||
          t == Type::Modifier_Any_Word || t == Type::Value_BX_Word ||
          t == Type::Value_BX_Offset_Word || t == Type::Value_BP_WordOffset ||
          t == Type::Value_WordAddress_Word || t == Type::Value_SI_Word ||
          t == Type::Value_SI_Offset_Word);
}

u8 Instruction::GetLength(u8 mod)
{
  u8 length = 0;

  u8 mod_bits = (mod & 0xC0) >> 3;
  u8 rm_bits = mod & 7;
  u8 mod_cmb = mod_bits | rm_bits;

  for (const auto& p : m_parameters) {
    if (ParameterNeedsResolving(p.GetType())) {
      using PType = Parameter::Type;
      switch (p.GetType()) {
      case PType::Literal_LongAddress_Immediate:
        length += sizeof(u32) - 1;
        break;
      case PType::Literal_Word:
        length += sizeof(u16);
        break;
      case PType::Literal_Byte:
      case PType::Literal_Word_Immediate:
      case PType::Literal_WordOffset:
      case PType::Value_WordAddress:
      case PType::Value_WordAddress_Word:
        length += sizeof(u8);
        break;
      case PType::Literal_Offset:
      case PType::Modifier_Register_Byte:
      case PType::Modifier_Register_Word:
      case PType::Modifier_Register_Segment:
      case PType::Literal_Byte_Immediate:
        break;
      case PType::Modifier_Any_Byte:
      case PType::Modifier_Any_Word:

        if (mod_cmb == 0b00'110) {
          length += sizeof(u16);
          break;
        }

        switch (mod_bits) {
        case 0b00'000:
        case 0b11'000:
          break;
        case 0b01'000:
          length += sizeof(u8);
          break;
        case 0b10'000:
          length += sizeof(u16);
          break;
        }
        break;
      default:
        std::cerr << "(IL) Don't know how to resolve "
                  << Core::CPU::ParameterTypeToString(p.GetType(), m_prefix)
                  << std::endl;
        throw UnhandledParameterException();
      }
    }
  }

  return length;
}

Instruction::Instruction(Type type) : m_type(type), m_offset(0) {}
Instruction::Instruction(const Instruction& ins, u8 opcode, u32 offset)
    : m_offset(offset)
{
  if (!ins.IsPrefix())
    throw "Error";

  *this = Instruction(opcode);

  switch (ins.GetType()) {
  case Type::PREFIX_CS:
    m_prefix = SegmentPrefix::CS;
    break;
  case Type::PREFIX_DS:
    m_prefix = SegmentPrefix::DS;
    break;
  case Type::PREFIX_ES:
    m_prefix = SegmentPrefix::ES;
    break;
  case Type::PREFIX_SS:
    m_prefix = SegmentPrefix::SS;
    break;
  default:
    m_prefix = SegmentPrefix::None;
  }
}

Instruction::Type Instruction::GetType() const { return m_type; }

Instruction::SegmentPrefix Instruction::GetPrefix() const { return m_prefix; }

std::string Instruction::ToString() const
{
  std::string disasm = TypeToString(m_type) + " ";

  bool no_parameters = true;

  for (const auto& p : m_parameters) {
    if (p.GetType() == Parameter::Type::None)
      break;
    else
      no_parameters = false;

    disasm += p.ToString(m_prefix, m_offset);
    disasm += ", ";
  }

  if (no_parameters)
    return disasm;
  else
    return disasm.substr(0, disasm.size() - 2);
}

void Instruction::AddParameter(Parameter parameter)
{
  if (parameter.GetType() == Parameter::Type::None)
    return;

  m_parameters.push_back(parameter);
}

const std::vector<Instruction::Parameter>& Instruction::GetParameters() const
{
  return m_parameters;
}

Instruction::Parameter& Instruction::GetParameter(size_t index)
{
  return m_parameters[index];
}
