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

  parser.AddString("boot");

  if (!parser.Parse(argc, argv)) {
    std::cerr << "Bad parameters provided. See --help" << std::endl;
    return 0;
  }

  QApplication app(argc, argv);

  MainWindow win(parser.GetString("boot"));
  win.show();

  return app.exec();
}
