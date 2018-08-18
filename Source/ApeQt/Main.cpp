// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <QApplication>

#include "ApeQt/MainWindow.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  MainWindow win(argc > 1 ? argv[1] : "");
  win.show();

  return app.exec();
}
