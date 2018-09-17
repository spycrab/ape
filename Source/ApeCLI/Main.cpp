// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <iostream>

#include "Core/Machine.h"
#include "Version.h"

#include "Common/ParameterParser.h"

int main(int argc, char** argv)
{
  std::cout << "Ape " << VERSION_STRING << " (c) Ape Emulator Project, 2018"
            << std::endl
            << std::endl;

  ParameterParser p;

  p.AddString("floppy");
  p.AddString("com");
  p.AddCommand("help");

  if (!p.Parse(argc, argv)) {
    std::cerr << "Bad parameters provided. See --help" << std::endl;
    return 1;
  }

  if (p.CheckCommand("help")) {
    std::cerr << argv[0] << " (floppy image) [floppy/com] [boot parameters]"
              << std::endl;
    return 1;
  }

  Core::Machine machine;

  if (p.GetString("floppy") != "") {
    auto& drive = machine.GetFloppyDrive();

    if (!drive.Insert(p.GetString("floppy"))) {
      std::cerr << "Failed to mount floppy image " << argv[1] << "!"
                << std::endl;
      return 1;
    }

    if (!drive.IsBootable()) {
      std::cerr << argv[1] << " is not a bootable floppy image." << std::endl;
      return 1;
    }

    return machine.BootFloppy() ? 0 : 1;
  } else if (p.GetString("floppy") != "") {
    return !machine.BootCOM(p.GetString("floppy"));
  }

  std::cerr << "Nothing to do! See --help" << std::endl;

  return 1;
}
