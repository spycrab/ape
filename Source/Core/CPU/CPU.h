// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include <atomic>
#include <functional>

#include "Common/Logger.h"
#include "Common/String.h"
#include "Common/Types.h"

#include "Core/CPU/Exception.h"
#include "Core/CPU/Instruction.h"
#include "Core/Memory.h"

//! Representation of the Central Processing Unit
namespace Core::CPU
{
//! Execute one CPU cycle
void Tick();

//! Execute instructions until shutdown is requested
void Start();

enum class RepeatMode : u8 { None, Repeat, Repeat_Zero, Repeat_Non_Zero };
enum class State : u8 { Stopped, Running, Paused };

using StateCallbackFunc = std::function<void(State)>;

void RegisterStateChangedCallback(StateCallbackFunc fnc);
void UnregisterStateChangedCallback(StateCallbackFunc fnc);

union GPR {
  u16 X = 0;
  struct {
    u8 L;
    u8 H;
  } b8;
};

extern GPR A;
extern GPR B;
extern GPR C;
extern GPR D;

//! AX (Accumulator)
extern u16& AX;
//! AH (High)
extern u8& AH;
//! AL (Low)
extern u8& AL;

//! BX
extern u16& BX;
//! BH (High)
extern u8& BH;
//! BL (Low)
extern u8& BL;

//! CX
extern u16& CX;
//! CH (High)
extern u8& CH;
//! CL (Low)
extern u8& CL;

//! DX
extern u16& DX;
//! DH (High)
extern u8& DH;
//! DL (Low)
extern u8& DL;

//! Code Segment
extern u16 CS;
//! Data Segment
extern u16 DS;
//! Extra(?) Segment
extern u16 ES;
//! Stack Segment
extern u16 SS;

//! Instruction Pointer
extern u16 IP;
//! Base Pointer
extern u16 BP;
//! Stack Pointer
extern u16 SP;
//! Source Index
extern u16 SI;
//! Destination Index
extern u16 DI;

//! Adjust Flag
extern bool AF;
//! Carry Flag
extern bool CF;
//! Interrupt Flag
extern bool IF;
//! Direction Flag
extern bool DF;
//! Overflow Flag
extern bool OF;
//! Parity Flag
extern bool PF;
//! Sign Flag
extern bool SF;
//! Zero Flag
extern bool ZF;

//! Simulate MS-DOS (Handle its interrupts)
extern bool simulate_msdos;

//! Stop the CPU
void Stop();

void SetPaused(bool paused);

bool IsRunning();
bool IsPaused();
State GetState();

extern u64 clock_speed;

u16 PrefixToValue(Instruction::SegmentPrefix prefix);

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
      throw ParameterLengthMismatchException(parameter);
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
      return Memory::Get<u8>(seg_val, parameter.GetData<u16>());
    case Type::Value_BP_Offset:
      return Memory::Get<u8>(seg_val, BP + parameter.GetData<u8>());
    case Type::Value_BP_WordOffset:
      return Memory::Get<u8>(seg_val, BP + parameter.GetData<u16>());
    case Type::Value_BP_DI:
      return Memory::Get<u8>(seg_val, BP + DI);
    case Type::Value_BP_DI_Offset:
      return Memory::Get<u8>(seg_val, BP + DI + parameter.GetData<u8>());
    case Type::Value_BP_DI_WordOffset:
      return Memory::Get<u8>(seg_val, BP + DI + parameter.GetData<u16>());
    case Type::Value_BP_SI:
      return Memory::Get<u8>(seg_val, BP + SI);
    case Type::Value_BP_SI_Offset:
      return Memory::Get<u8>(seg_val, BP + SI + parameter.GetData<u8>());
    case Type::Value_BP_SI_WordOffset:
      return Memory::Get<u8>(seg_val, BP + SI + parameter.GetData<u16>());
    case Type::Value_BX:
      return Memory::Get<u8>(seg_val, BX);
    case Type::Value_BX_Offset:
      return Memory::Get<u8>(seg_val, BX + parameter.GetData<u8>());
    case Type::Value_BX_WordOffset:
      return Memory::Get<u8>(seg_val, BX + parameter.GetData<u16>());
    case Type::Value_BX_SI:
      return Memory::Get<u8>(seg_val, BX + SI);
    case Type::Value_BX_SI_Offset:
      return Memory::Get<u8>(seg_val, BX + SI + parameter.GetData<u8>());
    case Type::Value_BX_SI_WordOffset:
      return Memory::Get<u8>(seg_val, BX + SI + parameter.GetData<u16>());
    case Type::Value_BX_DI:
      return Memory::Get<u8>(seg_val, BX + DI);
    case Type::Value_BX_DI_Offset:
      return Memory::Get<u8>(seg_val, BX + DI + parameter.GetData<u8>());
    case Type::Value_BX_DI_WordOffset:
      return Memory::Get<u8>(seg_val, BX + DI + parameter.GetData<u16>());
    case Type::Value_DI:
      return Memory::Get<u8>(seg_val, DI);
    case Type::Value_DI_Offset:
      return Memory::Get<u8>(seg_val, DI + parameter.GetData<u8>());
    case Type::Value_DI_WordOffset:
      return Memory::Get<u8>(seg_val, DI + parameter.GetData<u16>());
    case Type::Value_SI:
      return Memory::Get<u8>(seg_val, SI);
    case Type::Value_SI_Offset:
      return Memory::Get<u8>(seg_val, SI + parameter.GetData<u8>());
    case Type::Value_SI_WordOffset:
      return Memory::Get<u8>(seg_val, SI + parameter.GetData<u16>());
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
      throw UnhandledParameterException(parameter);
    }
  } else if constexpr (std::is_same<T, u16>::value ||
                       std::is_same<T, u16&>::value) {
    if (!parameter.IsWord())
      throw ParameterLengthMismatchException(parameter);

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
      return Memory::Get<u16>(seg_val, parameter.GetData<u16>());
    case Type::Value_DI_Word:
      return Memory::Get<u16>(seg_val, DI);
    case Type::Value_DI_Offset_Word:
      return Memory::Get<u16>(seg_val, DI + parameter.GetData<u8>());
    case Type::Value_SI_Word:
      return Memory::Get<u16>(seg_val, SI);
    case Type::Value_SI_Offset_Word:
      return Memory::Get<u16>(seg_val, SI + parameter.GetData<u8>());
    case Type::Value_BP_Offset_Word:
      return Memory::Get<u16>(seg_val, BP + parameter.GetData<u8>());
    case Type::Value_BP_WordOffset_Word:
      return Memory::Get<u16>(seg_val, BP + parameter.GetData<u16>());
    case Type::Value_BP_DI_Word:
      return Memory::Get<u16>(seg_val, BP + DI);
    case Type::Value_BP_DI_Offset_Word:
      return Memory::Get<u16>(seg_val, BP + DI + parameter.GetData<u8>());
    case Type::Value_BP_DI_WordOffset_Word:
      return Memory::Get<u16>(seg_val, BP + DI + parameter.GetData<u16>());
    case Type::Value_BP_SI_Word:
      return Memory::Get<u16>(seg_val, BP + SI);
    case Type::Value_BP_SI_Offset_Word:
      return Memory::Get<u16>(seg_val, BP + SI + parameter.GetData<u8>());
    case Type::Value_BP_SI_WordOffset_Word:
      return Memory::Get<u16>(seg_val, BP + SI + parameter.GetData<u16>());
    case Type::Value_BX_Word:
      return Memory::Get<u16>(seg_val, BX);
    case Type::Value_BX_Offset_Word:
      return Memory::Get<u16>(seg_val, BX + parameter.GetData<u8>());
    case Type::Value_BX_DI_Word:
      return Memory::Get<u16>(seg_val, BX + DI);
    case Type::Value_BX_DI_Offset_Word:
      return Memory::Get<u16>(seg_val, BX + DI + parameter.GetData<u8>());
    case Type::Value_BX_DI_WordOffset_Word:
      return Memory::Get<u16>(seg_val, BX + DI + parameter.GetData<u16>());
    case Type::Value_BX_SI_Word:
      return Memory::Get<u16>(seg_val, BX + SI);
    case Type::Value_BX_SI_Offset_Word:
      return Memory::Get<u16>(seg_val, BX + SI + parameter.GetData<u8>());
    case Type::Value_BX_SI_WordOffset_Word:
      return Memory::Get<u16>(seg_val, BX + SI + parameter.GetData<u16>());

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
          throw UnhandledParameterException(parameter);
          break;
        }
      } else {
        LOG("[WORD] Unknown type: " +
            ParameterTypeToString(parameter.GetType()));
        throw UnhandledParameterException(parameter);
      }
    }
  }
}

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
void JNO(const Instruction& instruction);

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
//! \endcond PRIVATE

void UpdateZF(u16 value);
void UpdatePF(u16 value);
void UpdateSF(i16 value);
template <typename T> void UpdateOF(i32 value);
template <typename T> void UpdateCF(i32 value);

void CallInterrupt(u8 vector);
bool CallBIOSInterrupt(u8 vector);
bool CallMSDOSInterrupt(u8 vector);
} // namespace Core::CPU
