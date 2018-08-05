// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/MainWindow.h"

#include <cstdlib>
#include <ctime>
#include <memory>
#include <thread>

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include "Version.h"

#include "ApeQt/TTYWidget.h"

#include "Core/Machine.h"

std::unique_ptr<Core::Machine> s_machine = nullptr;
std::thread s_thread;

QString MainWindow::GetQuote() const
{
  const QStringList list{tr("Less FPS than DOSBox"),
                         tr("Garuantee void if opened"),
                         tr("May contain traces of nuts"),
                         tr("NOP? NOP!"),
                         tr("Realest Mode of them all"),
                         tr("640K ought to be enough for everyone"),
                         tr("Just works (sometimes)"),
                         tr("MOV UP, ME ; Scotty"),
                         tr("Big Blue is coming for you")};

  srand(time(NULL));
  return list[random() % list.size()];
}

MainWindow::MainWindow()
{
  setWindowTitle(tr("Ape %1 - %2!").arg(VERSION_STRING).arg(GetQuote()));

  CreateWidgets();
  ConnectWidgets();

  resize(800, 600);

  if (s_machine != nullptr)
    s_machine->Shutdown();
}

MainWindow::~MainWindow()
{
  if (s_thread.joinable())
    s_thread.join();
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
  if (s_thread.joinable())
    s_thread.join();

  const QString& path =
      QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                   tr("Floppy Image(*.img);; COM File(*.com)"));

  if (path.isEmpty())
    return;

  s_machine.reset(new Core::Machine);

  if (path.endsWith(".com", Qt::CaseInsensitive)) {
    s_thread = std::thread([path] { s_machine->BootCOM(path.toStdString()); });
    return;
  }
  auto& drive = s_machine->GetFloppyDrive();

  if (!drive.Insert(path.toStdString())) {
    QMessageBox::critical(this, tr("Error"), tr("Failed to mount floppy!"));
    return;
  }

  if (!drive.IsBootable()) {
    QMessageBox::critical(this, tr("Error"),
                          tr("The provided disk is not bootable!"));
    return;
  }

  s_thread = std::thread([path] { s_machine->BootFloppy(); });
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
