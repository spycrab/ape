// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include "Common/Types.h"
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
  bool AF;
  //! Carry Flag
  bool CF;
  //! Interrupt Flag
  bool IF = true;
  //! Direction Flag
  bool DF = false;
  //! Overflow Flag
  bool OF;
  //! Parity Flag
  bool PF;
  //! Sign Flag
  bool SF;
  //! Zero Flag
  bool ZF;

  //! Simulate MS-DOS (Handle its interrupts)
  bool simulate_msdos = false;

private:
  //! \cond PRIVATE
  template <class T>
  T ParameterTo(const Instruction::Parameter& parameter,
                Instruction::SegmentPrefix prefix);

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

  void JPE(const Instruction& instruction);

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
