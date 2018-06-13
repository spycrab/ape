// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once
//! \file

#include <map>
#include <string>
#include <vector>

#include "Common/Types.h"

namespace Core
{
namespace CPU
{
//! High-Level representation of a instruction
class Instruction
{
public:
  /**@brief Enum of all possible segment prefixes
   *
   * These prefixes override the default segment of an instruction.
   */
  enum class SegmentPrefix : u8 {
    //! \cond PRIVATE
    None,
    CS,
    DS,
    ES,
    SS
    //! \endcond
  };

  //! Determines the type of the instruction.
  enum class Type : u8 {
    //! \cond PRIVATE

    // Instruction flow
    JMP,
    JZ,
    JNZ,
    JC,
    JNC,
    JCXZ,
    JPE,
    JNS,
    JNO,
    JNE,
    JL,
    JLE,
    JG,
    JGE,
    JA,
    JB,
    JNB,
    JPO,
    JO,
    JS,
    JBE,

    CALL,
    RET,
    IRET,
    RETF,
    REPNZ,
    REPZ,
    HLT,
    LOOPNZ,
    LOOPZ,
    LOOP,

    // Flags
    CLC,
    STC,
    CLI,
    STI,
    CLD,
    STD,

    // Move
    MOV,
    LEA,
    LES,
    LDS,
    XCHG,

    // Stack
    PUSH,
    POP,
    PUSHF,
    POPF,

    // String
    LODSB,
    LODSW,
    SCASB,
    SCASW,
    STOSB,
    STOSW,
    CMPSB,
    CMPSW,
    MOVSB,
    MOVSW,

    // Mathematical operations
    CMP,
    INC,
    DEC,
    ADD,
    SUB,
    ADC,
    DIV,
    IDIV,
    MUL,
    IMUL,
    SBB,

    // Bitwise operations
    OR,
    XOR,
    AND,
    NEG,
    NOT,
    SHL,
    SHR,
    ROL,
    ROR,
    RCL,
    RCR,

    // Other
    IN,
    OUT,
    LOCK,
    XLAT,
    INT,
    INTO,
    CMC,
    AAA,
    AAD,
    AAM,
    AAS,
    DAA,
    DAS,
    TEST,
    LAHF,
    SAHF,
    WAIT,
    NOP,
    CWD,
    CBW,

    // Prefixes
    PREFIX_DS,
    PREFIX_SS,
    PREFIX_CS,
    PREFIX_ES,

    // Groups
    GRP1,
    GRP2,
    GRP3a,
    GRP3b,
    GRP4,
    GRP5,

    Invalid
    //! \endcond PRIVATE
  };

  //! High-Level representation of a instruction parameter
  class Parameter
  {
  public:
    //! Enum to determine the types of parameters
    enum class Type : u8 {
      //! \cond PRIVATE
      AL,
      AH,
      BL,
      BH,
      CL,
      CH,
      DL,
      DH,
      AX,
      BX,
      CX,
      DX,
      CS,
      DS,
      ES,
      SS,
      IP,
      BP,
      SP,
      DI,
      SI,

      Literal_Byte,
      Literal_Byte_Immediate,
      Literal_Word,
      Literal_Word_Immediate,
      Literal_Offset,
      Literal_WordOffset,
      Literal_LongAddress_Immediate,

      Implied_0,
      Implied_1,
      Implied_3,

      Modifier_Register_Byte,
      Modifier_Register_Word,
      Modifier_Register_Segment,
      Modifier_Any_Byte,
      Modifier_Any_Word,

      // TEMPORARY
      None,

      // Later in resolvement
      Value_BP_Offset,
      Value_BP_Offset_Word,
      Value_BP_WordOffset,
      Value_BP_WordOffset_Word,
      Value_BP_SI,
      Value_BP_SI_Word,
      Value_BP_SI_Offset,
      Value_BP_SI_Offset_Word,
      Value_BP_SI_WordOffset,
      Value_BP_SI_WordOffset_Word,
      Value_BP_DI,
      Value_BP_DI_Word,
      Value_BP_DI_Offset,
      Value_BP_DI_Offset_Word,
      Value_BP_DI_WordOffset,
      Value_BP_DI_WordOffset_Word,

      Value_BX,
      Value_BX_Word,
      Value_BX_Offset,
      Value_BX_Offset_Word,
      Value_BX_WordOffset,
      Value_BX_WordOffset_Word,
      Value_BX_SI,
      Value_BX_SI_Word,
      Value_BX_SI_Offset,
      Value_BX_SI_Offset_Word,
      Value_BX_SI_WordOffset,
      Value_BX_SI_WordOffset_Word,
      Value_BX_DI,
      Value_BX_DI_Word,
      Value_BX_DI_Offset,
      Value_BX_DI_Offset_Word,
      Value_BX_DI_WordOffset,
      Value_BX_DI_WordOffset_Word,

      Value_SI,
      Value_SI_Word,
      Value_SI_Offset,
      Value_SI_Offset_Word,
      Value_SI_WordOffset,
      Value_SI_WordOffset_Word,

      Value_DI,
      Value_DI_Word,
      Value_DI_Offset,
      Value_DI_Offset_Word,
      Value_DI_WordOffset,
      Value_DI_WordOffset_Word,

      Value_WordAddress,
      Value_WordAddress_Word,

      //! \endcond PRIVATE
    };

    //! Create a parameter with the Instruction::Parameter::Type provided
    explicit Parameter(Parameter::Type type);

    //! Provide the parameter with its missing data
    void Resolve(u32 data);
    //! Change the type of the parameter and provide missing data (if any)
    void Resolve(Parameter::Type type, u32 data = 0);

    //! Get the parameters type
    Type GetType() const;

    //! Get the data associated with this parameter
    template <typename T> T GetData() const
    {
      return *reinterpret_cast<const T*>(&m_data);
    }

    //! Returns ``true`` if this parameter is resolved
    bool IsResolved() const;

    //! Returns ``true`` if this parameter points to or is a word
    bool IsWord() const;

    //! Get a human readable form of this parameter
    std::string ToString(SegmentPrefix prefix = SegmentPrefix::None,
                         u32 offset = 0) const;

  private:
    Type m_type;
    bool m_resolved = false;
    u32 m_data = 0;
  };

  Instruction() = default;

  /**
   * @brief Turns the provided opcode into an Instruction
   * @param opcode Opcode to be decoded
   */
  explicit Instruction(u8 opcode, u32 offset = 0);

  //! One but with prefixes
  explicit Instruction(const Instruction& ins, u8 opcode, u32 offset = 0);

  //! Get the Type associated with this instruction
  Type GetType() const;

  //! Get the SegmentPrefix associated with this instruction
  SegmentPrefix GetPrefix() const;

  //! Checks whether this Instruction needs further resolving
  bool IsResolved();

  //! Checks if this instruction is actually a prefix
  bool IsPrefix() const;

  /**
   * @brief Resolves the Instruction
   * @param mod Modifier byte. The byte after the opcode regardless if it is
   * used as such
   * @param data needs to have as many bytes as specified in
   * GetInstructionLength()
   * @return Returns ``false`` if there was an error during resolving.
   */
  bool Resolve(u8 mod, std::vector<u8> data);

  /**
   * @brief Get the length of the instruction provided
   * @return Length of the instruction in bytes
   */
  u8 GetLength(u8 mod);

  //! Get a disassembly for the Instruction provided
  std::string ToString() const;

  //! Get a vector of parameters
  const std::vector<Parameter>& GetParameters() const;

  //! Get a specific vector
  Parameter& GetParameter(size_t index);

  //! Add a new parameter
  void AddParameter(Parameter parameter);

private:
  Instruction(Instruction::Type type);

  std::vector<Parameter> m_parameters;
  Type m_type = Type::Invalid;
  SegmentPrefix m_prefix = SegmentPrefix::None;
  u32 m_offset = 0;
};

//! Get the corresponding nmoroic for the Type provided
std::string TypeToString(const Instruction::Type& type);

//! Get a human readable form of the Parameter::Type provided
std::string ParameterTypeToString(
    const Instruction::Parameter::Type& type,
    Instruction::SegmentPrefix prefix = Instruction::SegmentPrefix::None);

//! Checks whether this Parameter::Type needs resolving
bool ParameterNeedsResolving(const Instruction::Parameter::Type& parameter);
} // namespace CPU
} // namespace Core
