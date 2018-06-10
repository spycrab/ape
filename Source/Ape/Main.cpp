// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <iostream>

#include "Core/Machine.h"
#include "Version.h"

int main(int argc, char** argv)
{
  std::cout << "Ape " << VERSION_STRING << " (c) spycrab0, 2018" << std::endl
            << std::endl;

  if (argc != 2) {
    std::cerr << argv[0] << " (floppy image)" << std::endl;
    return 1;
  }

  Core::Machine machine;
  auto& drive = machine.GetFloppyDrive();

  if (!drive.Insert(argv[1])) {
    std::cerr << "Failed to mount floppy image " << argv[1] << "!" << std::endl;
    return 1;
  }

  if (!drive.IsBootable()) {
    std::cerr << argv[1] << " is not a bootable floppy image." << std::endl;
    return 1;
  }

  machine.Boot();

  return 0;
}
