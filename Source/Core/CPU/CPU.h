// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include <atomic>

#include "Common/Logger.h"
#include "Common/String.h"
#include "Common/Types.h"

#include "Core/CPU/Exception.h"
#include "Core/CPU/Instruction.h"
#include "Core/Memory.h"

namespace Core
{
class Machine;
}

namespace Core::CPU
{
//! Representation of the Central Processing Unit
class CPU
{
public:
  explicit CPU(Machine* machine);

  //! Execute one CPU cycle
  void Tick();

  //! Execute instructions until shutdown is requested
  void Start();

  //! AX (Accumulator)
  u16& AX = AX_struct.AX;
  //! AH (High)
  u8& AH = AX_struct.b8.AH;
  //! AL (Low)
  u8& AL = AX_struct.b8.AL;

  //! BX
  u16& BX = BX_struct.BX;
  //! BH (High)
  u8& BH = BX_struct.b8.BH;
  //! BL (Low)
  u8& BL = BX_struct.b8.BL;

  //! CX
  u16& CX = CX_struct.CX;
  //! CH (High)
  u8& CH = CX_struct.b8.CH;
  //! CL (Low)
  u8& CL = CX_struct.b8.CL;

  //! DX
  u16& DX = DX_struct.DX;
  //! DH (High)
  u8& DH = DX_struct.b8.DH;
  //! DL (Low)
  u8& DL = DX_struct.b8.DL;

  //! Code Segment
  u16 CS = 0;
  //! Data Segment
  u16 DS = 0;
  //! Extra(?) Segment
  u16 ES = 0;
  //! Stack Segment
  u16 SS = 0;

  //! Instruction Pointer
  u16 IP = 0;
  //! Base Pointer
  u16 BP = 0;
  //! Stack Pointer
  u16 SP = 0;
  //! Source Index
  u16 SI = 0;
  //! Destination Index
  u16 DI = 0;

  //! Adjust Flag
  bool AF = false;
  //! Carry Flag
  bool CF = false;
  //! Interrupt Flag
  bool IF = true;
  //! Direction Flag
  bool DF = false;
  //! Overflow Flag
  bool OF = false;
  //! Parity Flag
  bool PF = false;
  //! Sign Flag
  bool SF = false;
  //! Zero Flag
  bool ZF = false;

  //! Simulate MS-DOS (Handle its interrupts)
  bool simulate_msdos = false;

  //! Set whether the CPU is running
  std::atomic<bool> running = false;

private:
  //! \cond PRIVATE
  template <class T>
  T ParameterTo(const Instruction::Parameter& parameter,
                Instruction::SegmentPrefix prefix)
  {
    [[maybe_unused]] u16 seg_val = PrefixToValue(prefix);

    if constexpr (std::is_same<T, i8>::value) {
      return static_cast<i8>(ParameterTo<u8>(parameter, prefix));
    } else if constexpr (std::is_same<T, i8&>::value) {
      return *reinterpret_cast<i8*>(&ParameterTo<u8&>(parameter, prefix));
    } else if constexpr (std::is_same<T, i16>::value) {
      return static_cast<i16>(ParameterTo<u16>(parameter, prefix));
    } else if constexpr (std::is_same<T, i16&>::value) {
      return *reinterpret_cast<i16*>(&ParameterTo<u16&>(parameter, prefix));
    } else {
      static_assert(std::is_same<T, u8>::value || std::is_same<T, u8&>::value ||
                        std::is_same<T, u16>::value ||
                        std::is_same<T, u16&>::value,
                    "Bad type provided!");
    }

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

        LOG("[BYTE] Unknown type: " +
            ParameterTypeToString(parameter.GetType()));
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

  u16 PrefixToValue(Instruction::SegmentPrefix prefix);

  bool HandleRepetition();

  //// Arithmetic
  void ADC(const Instruction& instruction);
  void ADD(const Instruction& instruction);
  void DIV(const Instruction& instruction);
  void SBB(const Instruction& instruction);
  void SUB(const Instruction& instruction);

  void INC(const Instruction& instruction);
  void DEC(const Instruction& instruction);

  //// Jumps
  void JMP(const Instruction& instruction);

  void JA(const Instruction& instruction);

  void JB(const Instruction& instruction);
  void JBE(const Instruction& instruction);
  void JNB(const Instruction& instruction);

  void JCXZ(const Instruction& instruction);

  void JG(const Instruction& instruction);
  void JGE(const Instruction& instruction);

  void JL(const Instruction& instruction);
  void JLE(const Instruction& instruction);

  void JO(const Instruction& instruction);

  void JPE(const Instruction& instruction);
  void JPO(const Instruction& instruction);

  void JS(const Instruction& instruction);
  void JNS(const Instruction& instruction);

  void JZ(const Instruction& instruction);
  void JNZ(const Instruction& instruction);

  void CALL(const Instruction& instruction);
  void RET(const Instruction& instruction);

  //// Bitwise operations
  void AND(const Instruction& instruction);
  void TEST(const Instruction& instruction);
  void OR(const Instruction& instruction);
  void XOR(const Instruction& instruction);
  // Bitwise shifts
  void ROL(const Instruction& instruction);
  void ROR(const Instruction& instruction);
  void SHL(const Instruction& instruction);
  void SHR(const Instruction& instruction);

  //// String
  void CMPSB(const Instruction& instruction);
  void CMPSW(const Instruction& instruction);

  void LODSB(const Instruction& instruction);
  void LODSW(const Instruction& instruction);

  void MOVSB(const Instruction& instruction);
  void MOVSW(const Instruction& instruction);

  void STOSB(const Instruction& instruction);
  void STOSW(const Instruction& instruction);

  union {
    u16 AX = 0;
    struct {
      u8 AL;
      u8 AH;
    } b8;
  } AX_struct;
  union {
    u16 BX = 0;
    struct {
      u8 BL;
      u8 BH;
    } b8;
  } BX_struct;
  union {
    u16 CX = 0;
    struct {
      u8 CL;
      u8 CH;
    } b8;
  } CX_struct;
  union {
    u16 DX = 0;
    struct {
      u8 DL;
      u8 DH;
    } b8;
  } DX_struct;
  //! \endcond PRIVATE

  void UpdateZF(u16 value);
  void UpdatePF(u16 value);
  void UpdateSF(i16 value);
  template <typename T> void UpdateOF(i32 value);
  template <typename T> void UpdateCF(i32 value);

  void CallInterrupt(u8 vector);
  bool CallBIOSInterrupt(u8 vector);
  bool CallMSDOSInterrupt(u8 vector);

  enum class RepeatMode : u8 { None, Repeat, Repeat_Zero, Repeat_Non_Zero };

  Core::Machine* m_machine;
  Core::Memory& m_memory;
  RepeatMode m_repeat_mode;
};
} // namespace Core::CPU
