// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <iostream>

#include <QApplication>

#include "Common/ParameterParser.h"

#include "ApeQt/MainWindow.h"

int main(int argc, char** argv)
{
  ParameterParser parser;

  parser.AddString("com");
  parser.AddString("floppy");
  parser.AddCommand("help");

  if (!parser.Parse(argc, argv)) {
    std::cerr << "Bad parameters provided. See --help" << std::endl;
    return 0;
  }

  if (parser.CheckCommand("help")) {
    std::cerr << argv[0] << " (--floppy/--com) [file]" << std::endl;

    return 1;
  }

  QApplication app(argc, argv);

  MainWindow win(parser.GetString("com").empty() ? parser.GetString("floppy")
                                                 : parser.GetString("com"),
                 parser.GetString("com").empty());
  win.show();

  return app.exec();
}
