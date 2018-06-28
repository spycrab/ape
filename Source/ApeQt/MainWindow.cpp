// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/MainWindow.h"

#include <cstdlib>
#include <ctime>

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include "Version.h"

#include "ApeQt/TTYWidget.h"

#include "Core/Machine.h"

QString MainWindow::GetQuote() const
{
  const QStringList list{tr("Less FPS than DOSBox"),
                         tr("Garuantee void if opened"),
                         tr("May contain traces of nuts")};

  srand(time(NULL));
  return list[random() % list.size()];
}

MainWindow::MainWindow()
{
  setWindowTitle(tr("Ape %1 - %2!").arg(VERSION_STRING).arg(GetQuote()));

  CreateWidgets();
  ConnectWidgets();
}

void MainWindow::CreateWidgets()
{
  m_menu_bar = new QMenuBar;

  auto* file_menu = m_menu_bar->addMenu(tr("File"));

  file_menu->addAction(tr("Open..."), this, &MainWindow::OpenFile,
                       QKeySequence("Ctrl+O"));
  file_menu->addAction(tr("Exit"), this, &MainWindow::close,
                       QKeySequence("Alt+F4"));

  auto* help_menu = m_menu_bar->addMenu(tr("Help"));

  help_menu->addAction(tr("About..."), this, &MainWindow::ShowAbout);

  setMenuBar(m_menu_bar);

  setCentralWidget(new TTYWidget);
}

void MainWindow::ConnectWidgets() {}

void MainWindow::OpenFile()
{
  const QString& path =
      QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                   tr("Floppy Image(*.img);; COM File(*.com)"));

  if (path.isEmpty())
    return;

  Core::Machine machine;

  auto& drive = machine.GetFloppyDrive();

  drive.Insert(path.toStdString());

  machine.BootFloppy();
}

void MainWindow::ShowAbout()
{
  QMessageBox::information(
      this, tr("About Ape"),
      tr("Version: %1\n\nApe is an experimental IBM PC emulator written in "
         "C++17.\n\nApe is "
         "licensed under the GNU GPL v3 or any later version at your "
         "option. See LICENSE.")
          .arg(VERSION_STRING));
}
