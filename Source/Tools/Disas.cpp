// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Common/Logger.h"
#include "Common/String.h"
#include "Common/Types.h"
#include "Version.h"

#include "Core/CPU/Instruction.h"

using namespace Core::CPU;

std::string ToDB(u8 byte)
{
  std::string c(1, static_cast<char>(byte));

  if (c == "'")
    c = "\\'";

  return std::isprint(static_cast<char>(byte)) ? "'" + c + "'"
                                               : String::ToHex(byte);
}

int main(int argc, char** argv)
{
  std::cerr << "Ape " << VERSION_STRING << " 8086 Disassembler" << std::endl
            << "(c) Ape Emulator Project, 2018" << std::endl
            << std::endl;

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " (file)" << std::endl;
    return 1;
  }
  std::ifstream ifs(argv[1], std::ios::binary);

  if (!ifs.good()) {
    ERROR("Failed to open " + std::string(argv[1]));
    return 1;
  }

  while (!ifs.eof()) {
    u32 address = static_cast<u32>(
        ifs.tellg()); // Address to bark at if anything goes wrong
    u8 opcode = static_cast<u8>(ifs.get());

    auto ins = Instruction(opcode, address);

    if (ins.IsPrefix())
      ins = Instruction(ins, ifs.get(), address);

    std::cout << String::ToHex(address) << " ";

    if (ins.GetType() == Instruction::Type::Invalid) {
      std::cout << "DB " << ToDB(opcode) << std::endl;
      continue;
    }

    if (!ins.IsResolved()) {
      u8 mod = static_cast<u8>(ifs.get());
      u8 length = ins.GetLength(mod);

      if (ins.GetType() == Instruction::Type::Invalid) {
        std::cout << "DB " << ToDB(opcode) << std::endl;
        continue;
      }

      std::vector<u8> data;
      data.resize(length);

      ifs.read(reinterpret_cast<char*>(data.data()), length);

      if (!ifs.good()) {
        ERROR("Unexpected EOF.");
        return 1;
      }

      if (!ins.Resolve(mod, data)) {
        ERROR("Warning: Failed to fully resolve parameters at " +
               String::ToHex(address) + ".");

        std::cout << String::ToHex(address + 1) << " ";
        std::cout << "DB " << ToDB(opcode) << std::endl;
        std::cout << String::ToHex(address + 2) << " ";
        std::cout << "DB " << ToDB(mod) << std::endl;

        u8 offset = 2;

        for (u8 byte : data)
          std::cout << "DB " << ToDB(byte + offset++) << std::endl;

        continue;
      }
    }

    std::cout << ins.ToString() << std::endl;
  }

  return 0;
}
