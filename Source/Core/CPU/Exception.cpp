// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Common/String.h"

#include "Core/CPU/Exception.h"

using namespace Core::CPU;

CPUException::CPUException(const std::string& message)
    : std::runtime_error(message)
{
}

CPUException::CPUException() : std::runtime_error("Unknown exception occured")
{
}

UnhandledInstructionException::UnhandledInstructionException(
    const Instruction& ins)
    : CPUException("Don't know what to do with instruction type: " +
                   TypeToString(ins.GetType()))
{
}

InvalidParameterException::InvalidParameterException(u8 opcode, u8 mod)
    : CPUException("Failed to decode opcode " + String::ToHex(opcode) +
                   " with mod " + String::ToHex(mod))
{
}

InvalidInstructionException::InvalidInstructionException(u8 opcode)
    : CPUException("Hit an invalid instruction with the opcode " +
                   String::ToHex(opcode))
{
}

UnsupportedParameterException::UnsupportedParameterException(
    const Instruction& ins, const Instruction::Parameter& p)
    : CPUException("Instruction " + TypeToString(ins.GetType()) +
                   " does not support parameter " +
                   ParameterTypeToString(p.GetType()))
{
}

ParameterLengthMismatchException::ParameterLengthMismatchException(
    const Instruction& ins, const Instruction::Parameter& p1,
    const Instruction::Parameter& p2)
    : CPUException("Instruction " + TypeToString(ins.GetType()) +
                   " has received mismatching parameters" +
                   ParameterTypeToString(p1.GetType()) + " and " +
                   ParameterTypeToString(p2.GetType()))
{
}

ParameterLengthMismatchException::ParameterLengthMismatchException(
    const Instruction::Parameter& p)
    : CPUException("Parameter " + ParameterTypeToString(p.GetType()) +
                   " has been requested with the wrong length")
{
}

UnhandledParameterException::UnhandledParameterException(
    const Instruction::Parameter& p)
    : CPUException("Parameter " + ParameterTypeToString(p.GetType()) +
                   " is unhandled at this point in time")
{
}
