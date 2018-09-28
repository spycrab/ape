// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <memory>
#include <string>
#include <thread>

#include <QMainWindow>
#include <QString>

#include "Core/CPU/CPU.h"
#include "Core/CPU/Exception.h"
#include "Core/Machine.h"

class QAction;
class QLabel;
class QMenuBar;
class QStatusBar;

class MainWindow : public QMainWindow
{
public:
  explicit MainWindow(const std::string&& path = "");
  ~MainWindow();

private:
  void CreateWidgets();
  void ConnectWidgets();

  QString GetQuote() const;

  void OpenFile();
  void ShowAbout();

  void StartFile(const QString& path);

  void StopMachine();
  void PauseMachine();

  void HandleException(Core::CPU::CPUException e);
  void ShowStatus(const QString& status, int timeout = 0);

  void OnMachineStateChanged(Core::CPU::State state);

  QMenuBar* m_menu_bar;
  QAction* m_machine_stop;
  QAction* m_machine_pause;

  QStatusBar* m_status_bar;
  QLabel* m_status_label;

  std::thread m_thread;
};
