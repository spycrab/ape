// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/MainWindow.h"

#include <memory>
#include <random>

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>

#include "ApeQt/QueueOnObject.h"
#include "ApeQt/TTYWidget.h"

#include "Core/Machine.h"

#include "Version.h"

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
                         tr("Big Blue is coming for you"),
                         tr("Crashes in your general direction")};

  std::random_device rd;
  std::default_random_engine engine(rd());
  std::uniform_int_distribution<int> rnd(0, list.size() - 1);

  return list[rnd(engine)];
}

MainWindow::MainWindow(const std::string&& path)
{
  setWindowTitle(tr("Ape %1 - %2!").arg(VERSION_STRING).arg(GetQuote()));

  CreateWidgets();
  ConnectWidgets();

  resize(800, 600);

  StartFile(QString::fromStdString(path));
}

MainWindow::~MainWindow() { StopMachine(); }

void MainWindow::CreateWidgets()
{
  m_menu_bar = new QMenuBar;

  auto* file_menu = m_menu_bar->addMenu(tr("File"));

  file_menu->addAction(tr("Open..."), this, &MainWindow::OpenFile,
                       QKeySequence("Ctrl+O"));
  file_menu->addAction(tr("Exit"), this, &MainWindow::close,
                       QKeySequence("Alt+F4"));

  auto* machine_menu = m_menu_bar->addMenu(tr("Machine"));

  m_machine_stop = machine_menu->addAction(
      tr("Stop"), this, &MainWindow::StopMachine, QKeySequence("Ctrl+H"));

  m_machine_pause = machine_menu->addAction(tr("Pause / Resume"), this,
                                            &MainWindow::PauseMachine,
                                            QKeySequence("Ctrl+P"));

  m_machine_stop->setEnabled(false);
  m_machine_pause->setEnabled(false);

  auto* help_menu = m_menu_bar->addMenu(tr("Help"));

  help_menu->addAction(tr("About..."), this, &MainWindow::ShowAbout);

  setMenuBar(m_menu_bar);

  m_status_bar = new QStatusBar;

  ShowStatus(tr("Welcome to Ape!"), 5000);

  setStatusBar(m_status_bar);
}

void MainWindow::ConnectWidgets() {}

void MainWindow::OpenFile()
{
  if (m_thread.joinable())
    m_thread.join();

  const QString& path =
      QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                   tr("Floppy Image(*.img);; COM File(*.com)"));

  StartFile(path);
}

void MainWindow::StartFile(const QString& path)
{
  if (path.isEmpty())
    return;

  m_machine.reset(new Core::Machine);

  setCentralWidget(new TTYWidget(&m_machine->GetVGA()));

  if (path.endsWith(".com", Qt::CaseInsensitive)) {
    m_thread = std::thread([this, path] {
      OnMachineStateChanged(true);
      try {
        m_machine->BootCOM(path.toStdString());
      } catch (Core::CPU::CPUException& e) {
        HandleException(e);
      }
      OnMachineStateChanged(false);
    });
    return;
  }
  auto& drive = m_machine->GetFloppyDrive();

  if (!drive.Insert(path.toStdString())) {
    QMessageBox::critical(this, tr("Error"), tr("Failed to mount floppy!"));
    return;
  }

  if (!drive.IsBootable()) {
    QMessageBox::critical(this, tr("Error"),
                          tr("The provided disk is not bootable!"));
    return;
  }

  m_thread = std::thread([this, path] {
    OnMachineStateChanged(true);
    try {
      m_machine->BootFloppy();
    } catch (Core::CPU::CPUException& e) {
      HandleException(e);
    }
    OnMachineStateChanged(false);
  });
}

void MainWindow::StopMachine()
{
  if (m_machine == nullptr)
    return;

  m_machine->Stop();

  if (m_thread.joinable())
    m_thread.join();
}

void MainWindow::PauseMachine()
{
  m_machine->Pause();
  ShowStatus("Paused / Resumed");
}

void MainWindow::OnMachineStateChanged(bool running)
{
  QueueOnObject(this, [this, running] {
    m_machine_stop->setEnabled(running);
    m_machine_pause->setEnabled(running);
  });

  ShowStatus(running ? tr("Running...") : tr("Stopped"));
}

void MainWindow::HandleException(Core::CPU::CPUException e)
{
  QueueOnObject(this, [this, e] {
    QMessageBox::critical(
        this, tr("Error"),
        tr("A fatal error exception and emulation cannot continue:\n\n%1")
            .arg(QString::fromStdString(e.what())));
  });

  ShowStatus("Crashed :(");
}

void MainWindow::ShowStatus(const QString& message, int timeout)
{
  QueueOnObject(this, [this, message, timeout] {
    m_status_bar->showMessage(message, timeout);
  });
}

void MainWindow::ShowAbout()
{
  QMessageBox::information(
      this, tr("About Ape"),
      tr("Version: %1\n\nApe is an experimental IBM PC emulator written in "
         "C++17.\n\nApe is "
         "licensed under the GNU GPL v3 or any later version at your "
         "option. See LICENSE.\n\n(c) Ape Emulator Project, 2018")
          .arg(VERSION_STRING));
}
