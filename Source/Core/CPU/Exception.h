#pragma once
//! \file

#include <exception>

namespace Core::CPU
{
//! Thrown when the CPU hits an instruction that couldn't be resolved / is
//! invalid
class InvalidInstructionException : public std::exception
{
};

//! Thrown when the CPU hits an unimplemented Instruction::Type
class UnhandledInstructionException : public std::exception
{
};

//! Thrown when the CPU hits an unimplemented Instruction::Parameter::Type
class UnhandledParameterException : public std::exception
{
};

//! Thrown when a ``word`` and ``byte`` parameter are used together when they
//! shouldn't be
class ParameterLengthMismatchException : public std::exception
{
};

//! Thrown when a interrupt is neither handled by software nor hardware
class UnhandledInterruptException : public std::exception
{
};
}
