#pragma once
//! \file

#include <exception>

#include "Core/CPU/Instruction.h"

namespace Core::CPU
{

//! Base class thrown by every CPU related exception

class CPUException : public std::runtime_error
{
protected:
  CPUException();
  CPUException(const std::string& message);
};

//! Thrown when the CPU hits an instruction that couldn't be resolved / is
//! invalid
class InvalidInstructionException : public CPUException
{
public:
  InvalidInstructionException(u8 opcode);
};

//! Thrown when the CPU hits an unimplemented Instruction::Type
class UnhandledInstructionException : public CPUException
{
public:
  UnhandledInstructionException(const Instruction& ins);
};

//! Thrown when the CPU hits an unimplemented Instruction::Parameter::Type
class UnhandledParameterException : public CPUException
{
public:
  UnhandledParameterException(const Instruction::Parameter& p);
};

//! Thrown when the CPU hits an invalid mod byte that can't be decoded
class InvalidParameterException : public CPUException
{
public:
  InvalidParameterException(u8 opcode, u8 mod);
};

//! Thrown when a ``word`` and ``byte`` parameter are used together when they
//! shouldn't be
class ParameterLengthMismatchException : public CPUException
{
public:
  ParameterLengthMismatchException(const Instruction& ins,
                                   const Instruction::Parameter& p1,
                                   const Instruction::Parameter& p2);

  ParameterLengthMismatchException(const Instruction::Parameter& p);
};

//! Thrown when a instruction doesn't support the parameter type provided
class UnsupportedParameterException : public CPUException
{
public:
  UnsupportedParameterException(const Instruction& ins,
                                const Instruction::Parameter& p);
};

//! Thrown when a interrupt is neither handled by software nor hardware
class UnhandledInterruptException : public CPUException
{
};
} // namespace Core::CPU
