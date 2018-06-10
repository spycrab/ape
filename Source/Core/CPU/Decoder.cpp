// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/Instruction.h"

using namespace Core::CPU;

Core::CPU::Instruction::Instruction(u8 opcode, u32 offset)
{
  static bool init_required = true;
  static std::map<u8, Core::CPU::Instruction> instructions;

  if (init_required) {
    auto reg_op = [&](u8 opcode, Type type,
                      Parameter::Type t1 = Parameter::Type::None,
                      Parameter::Type t2 = Parameter::Type::None) {
      Core::CPU::Instruction i(type);

      i.AddParameter(Parameter(t1));
      i.AddParameter(Parameter(t2));

      instructions[opcode] = std::move(i);
    };

    reg_op(0x00, Type::ADD, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x01, Type::ADD, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x02, Type::ADD, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x03, Type::ADD, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x04, Type::ADD, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x05, Type::ADD, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x06, Type::PUSH, Parameter::Type::ES);
    reg_op(0x07, Type::POP, Parameter::Type::ES);
    reg_op(0x08, Type::OR, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x09, Type::OR, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x0A, Type::OR, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x0B, Type::OR, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x0C, Type::OR, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x0D, Type::OR, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x0E, Type::PUSH, Parameter::Type::CS);

    reg_op(0x10, Type::ADC, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x11, Type::ADC, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x12, Type::ADC, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x13, Type::ADC, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x14, Type::ADC, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x15, Type::ADC, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x16, Type::PUSH, Parameter::Type::SS);
    reg_op(0x17, Type::POP, Parameter::Type::SS);
    reg_op(0x18, Type::SBB, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x19, Type::SBB, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x1A, Type::SBB, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x1B, Type::SBB, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x1C, Type::SBB, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x1D, Type::SBB, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x1E, Type::PUSH, Parameter::Type::DS);
    reg_op(0x1F, Type::POP, Parameter::Type::DS);

    reg_op(0x20, Type::AND, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x21, Type::AND, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x22, Type::AND, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x23, Type::AND, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x24, Type::AND, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x25, Type::AND, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x26, Type::PREFIX_ES);
    reg_op(0x27, Type::DAA);
    reg_op(0x28, Type::SUB, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x29, Type::SUB, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x2A, Type::SUB, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x2B, Type::SUB, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x2C, Type::SUB, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x2D, Type::SUB, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x2E, Type::PREFIX_CS);
    reg_op(0x2F, Type::DAS);

    reg_op(0x30, Type::XOR, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x31, Type::XOR, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x32, Type::XOR, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x33, Type::XOR, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x34, Type::XOR, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x35, Type::XOR, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x36, Type::PREFIX_SS);
    reg_op(0x37, Type::AAA);
    reg_op(0x38, Type::CMP, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x39, Type::CMP, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x3A, Type::CMP, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x3B, Type::CMP, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x3C, Type::CMP, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0x3D, Type::CMP, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0x3E, Type::PREFIX_DS);
    reg_op(0x3F, Type::AAS);

    reg_op(0x40, Type::INC, Parameter::Type::AX);
    reg_op(0x41, Type::INC, Parameter::Type::CX);
    reg_op(0x42, Type::INC, Parameter::Type::DX);
    reg_op(0x43, Type::INC, Parameter::Type::BX);
    reg_op(0x44, Type::INC, Parameter::Type::SP);
    reg_op(0x45, Type::INC, Parameter::Type::BP);
    reg_op(0x46, Type::INC, Parameter::Type::SI);
    reg_op(0x47, Type::INC, Parameter::Type::DI);
    reg_op(0x48, Type::DEC, Parameter::Type::AX);
    reg_op(0x49, Type::DEC, Parameter::Type::CX);
    reg_op(0x4A, Type::DEC, Parameter::Type::DX);
    reg_op(0x4B, Type::DEC, Parameter::Type::BX);
    reg_op(0x4C, Type::DEC, Parameter::Type::SP);
    reg_op(0x4D, Type::DEC, Parameter::Type::BP);
    reg_op(0x4E, Type::DEC, Parameter::Type::SI);
    reg_op(0x4F, Type::DEC, Parameter::Type::DI);

    reg_op(0x50, Type::PUSH, Parameter::Type::AX);
    reg_op(0x51, Type::PUSH, Parameter::Type::CX);
    reg_op(0x52, Type::PUSH, Parameter::Type::DX);
    reg_op(0x53, Type::PUSH, Parameter::Type::BX);
    reg_op(0x54, Type::PUSH, Parameter::Type::SP);
    reg_op(0x55, Type::PUSH, Parameter::Type::BP);
    reg_op(0x56, Type::PUSH, Parameter::Type::SI);
    reg_op(0x57, Type::PUSH, Parameter::Type::DI);
    reg_op(0x58, Type::POP, Parameter::Type::AX);
    reg_op(0x59, Type::POP, Parameter::Type::CX);
    reg_op(0x5A, Type::POP, Parameter::Type::DX);
    reg_op(0x5B, Type::POP, Parameter::Type::BX);
    reg_op(0x5C, Type::POP, Parameter::Type::SP);
    reg_op(0x5D, Type::POP, Parameter::Type::BP);
    reg_op(0x5E, Type::POP, Parameter::Type::SI);
    reg_op(0x5F, Type::POP, Parameter::Type::DI);

    reg_op(0x70, Type::JO, Parameter::Type::Literal_Offset);
    reg_op(0x71, Type::JNO, Parameter::Type::Literal_Offset);
    reg_op(0x72, Type::JB, Parameter::Type::Literal_Offset);
    reg_op(0x73, Type::JNB, Parameter::Type::Literal_Offset);
    reg_op(0x74, Type::JZ, Parameter::Type::Literal_Offset);
    reg_op(0x75, Type::JNZ, Parameter::Type::Literal_Offset);
    reg_op(0x76, Type::JBE, Parameter::Type::Literal_Offset);
    reg_op(0x77, Type::JA, Parameter::Type::Literal_Offset);
    reg_op(0x78, Type::JS, Parameter::Type::Literal_Offset);
    reg_op(0x79, Type::JNS, Parameter::Type::Literal_Offset);
    reg_op(0x7A, Type::JPE, Parameter::Type::Literal_Offset);
    reg_op(0x7B, Type::JPO, Parameter::Type::Literal_Offset);
    reg_op(0x7C, Type::JL, Parameter::Type::Literal_Offset);
    reg_op(0x7D, Type::JGE, Parameter::Type::Literal_Offset);
    reg_op(0x7E, Type::JLE, Parameter::Type::Literal_Offset);
    reg_op(0x7F, Type::JG, Parameter::Type::Literal_Offset);

    reg_op(0x80, Type::GRP1, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Literal_Byte);
    reg_op(0x81, Type::GRP1, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Literal_Word);
    reg_op(0x82, Type::GRP1, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Literal_Byte);
    reg_op(0x83, Type::GRP1, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Literal_Byte);
    reg_op(0x84, Type::TEST, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x85, Type::TEST, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x86, Type::XCHG, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x87, Type::XCHG, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x88, Type::MOV, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Modifier_Register_Byte);
    reg_op(0x89, Type::MOV, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Word);
    reg_op(0x8A, Type::MOV, Parameter::Type::Modifier_Register_Byte,
           Parameter::Type::Modifier_Any_Byte);
    reg_op(0x8B, Type::MOV, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x8C, Type::MOV, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Modifier_Register_Segment);
    reg_op(0x8D, Type::LEA, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x8E, Type::MOV, Parameter::Type::Modifier_Register_Segment,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0x8F, Type::POP, Parameter::Type::Modifier_Any_Word);

    reg_op(0x90, Type::NOP);
    reg_op(0x91, Type::XCHG, Parameter::Type::CX, Parameter::Type::AX);
    reg_op(0x92, Type::XCHG, Parameter::Type::DX, Parameter::Type::AX);
    reg_op(0x93, Type::XCHG, Parameter::Type::BX, Parameter::Type::AX);
    reg_op(0x94, Type::XCHG, Parameter::Type::SP, Parameter::Type::AX);
    reg_op(0x95, Type::XCHG, Parameter::Type::BP, Parameter::Type::AX);
    reg_op(0x96, Type::XCHG, Parameter::Type::SI, Parameter::Type::AX);
    reg_op(0x97, Type::XCHG, Parameter::Type::DI, Parameter::Type::AX);
    reg_op(0x98, Type::CBW);
    reg_op(0x99, Type::CWD);
    reg_op(0x9A, Type::CALL, Parameter::Type::Literal_LongAddress_Immediate);
    reg_op(0x9B, Type::WAIT);
    reg_op(0x9C, Type::PUSHF);
    reg_op(0x9D, Type::POPF);
    reg_op(0x9E, Type::SAHF);
    reg_op(0x9F, Type::LAHF);

    reg_op(0xA0, Type::MOV, Parameter::Type::AL,
           Parameter::Type::Value_WordAddress);
    reg_op(0xA1, Type::MOV, Parameter::Type::AX,
           Parameter::Type::Value_WordAddress_Word);
    reg_op(0xA2, Type::MOV, Parameter::Type::Value_WordAddress,
           Parameter::Type::AL);
    reg_op(0xA3, Type::MOV, Parameter::Type::Value_WordAddress_Word,
           Parameter::Type::AX);

    reg_op(0xA4, Type::MOVSB);
    reg_op(0xA5, Type::MOVSW);
    reg_op(0xA6, Type::CMPSB);
    reg_op(0xA7, Type::CMPSW);
    reg_op(0xA8, Type::TEST, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xA9, Type::TEST, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xAA, Type::STOSB);
    reg_op(0xAB, Type::STOSW);
    reg_op(0xAC, Type::LODSB);
    reg_op(0xAD, Type::LODSW);
    reg_op(0xAE, Type::SCASB);
    reg_op(0xAF, Type::SCASW);

    reg_op(0xB0, Type::MOV, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB1, Type::MOV, Parameter::Type::CL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB2, Type::MOV, Parameter::Type::DL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB3, Type::MOV, Parameter::Type::BL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB4, Type::MOV, Parameter::Type::AH,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB5, Type::MOV, Parameter::Type::CH,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB6, Type::MOV, Parameter::Type::DH,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB7, Type::MOV, Parameter::Type::BH,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xB8, Type::MOV, Parameter::Type::AX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xB9, Type::MOV, Parameter::Type::CX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xBA, Type::MOV, Parameter::Type::DX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xBB, Type::MOV, Parameter::Type::BX,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xBC, Type::MOV, Parameter::Type::SP,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xBD, Type::MOV, Parameter::Type::BP,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xBE, Type::MOV, Parameter::Type::SI,
           Parameter::Type::Literal_Word_Immediate);
    reg_op(0xBF, Type::MOV, Parameter::Type::DI,
           Parameter::Type::Literal_Word_Immediate);

    reg_op(0xC2, Type::RET, Parameter::Type::Literal_Word_Immediate);
    reg_op(0xC3, Type::RET);
    reg_op(0xC4, Type::LES, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0xC5, Type::LDS, Parameter::Type::Modifier_Register_Word,
           Parameter::Type::Modifier_Any_Word);
    reg_op(0xC6, Type::MOV, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Literal_Byte);
    reg_op(0xC7, Type::MOV, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Literal_Word);
    reg_op(0xCA, Type::RETF, Parameter::Type::Literal_Word_Immediate);
    reg_op(0xCB, Type::RETF);
    reg_op(0xCC, Type::INT, Parameter::Type::Implied_3);
    reg_op(0xCD, Type::INT, Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xCE, Type::INTO);
    reg_op(0xCF, Type::IRET);

    reg_op(0xD0, Type::GRP2, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::Implied_1);
    reg_op(0xD1, Type::GRP2, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::Implied_1);
    reg_op(0xD2, Type::GRP2, Parameter::Type::Modifier_Any_Byte,
           Parameter::Type::CL);
    reg_op(0xD3, Type::GRP2, Parameter::Type::Modifier_Any_Word,
           Parameter::Type::CL);
    reg_op(0xD4, Type::AAM, Parameter::Type::Implied_0);
    reg_op(0xD5, Type::AAD, Parameter::Type::Implied_0);
    reg_op(0xD7, Type::XLAT);

    reg_op(0xE0, Type::LOOPNZ, Parameter::Type::Literal_Offset);
    reg_op(0xE1, Type::LOOPZ, Parameter::Type::Literal_Offset);
    reg_op(0xE2, Type::LOOP, Parameter::Type::Literal_Offset);
    reg_op(0xE3, Type::JCXZ, Parameter::Type::Literal_Offset);
    reg_op(0xE4, Type::IN, Parameter::Type::AL,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xE5, Type::IN, Parameter::Type::AX,
           Parameter::Type::Literal_Byte_Immediate);
    reg_op(0xE6, Type::OUT, Parameter::Type::Literal_Byte_Immediate,
           Parameter::Type::AL);
    reg_op(0xE7, Type::OUT, Parameter::Type::Literal_Byte_Immediate,
           Parameter::Type::AX);
    reg_op(0xE8, Type::CALL, Parameter::Type::Literal_WordOffset);
    reg_op(0xE9, Type::JMP, Parameter::Type::Literal_WordOffset);
    reg_op(0xEA, Type::JMP, Parameter::Type::Literal_LongAddress_Immediate);
    reg_op(0xEB, Type::JMP, Parameter::Type::Literal_Offset);
    reg_op(0xEC, Type::IN, Parameter::Type::AL, Parameter::Type::DX);
    reg_op(0xED, Type::IN, Parameter::Type::AX, Parameter::Type::DX);
    reg_op(0xEE, Type::OUT, Parameter::Type::DX, Parameter::Type::AL);
    reg_op(0xEF, Type::OUT, Parameter::Type::DX, Parameter::Type::AX);

    reg_op(0xF0, Type::LOCK);
    reg_op(0xF2, Type::REPNZ);
    reg_op(0xF3, Type::REPZ);
    reg_op(0xF4, Type::HLT);
    reg_op(0xF5, Type::CMC);
    reg_op(0xF6, Type::GRP3a, Parameter::Type::Modifier_Any_Byte);
    reg_op(0xF7, Type::GRP3b, Parameter::Type::Modifier_Any_Word);
    reg_op(0xF8, Type::CLC);
    reg_op(0xF9, Type::STC);
    reg_op(0xFA, Type::CLI);
    reg_op(0xFB, Type::STI);
    reg_op(0xFC, Type::CLD);
    reg_op(0xFD, Type::STD);
    reg_op(0xFE, Type::GRP4, Parameter::Type::Modifier_Any_Byte);
    reg_op(0xFF, Type::GRP5, Parameter::Type::Modifier_Any_Word);
    init_required = false;
  }

  if (instructions.count(opcode) > 0)
    *this = instructions[opcode];
  else {
    Core::CPU::Instruction i(Type::Invalid);
    *this = i;
  }

  m_offset = offset;
}
