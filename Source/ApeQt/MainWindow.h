// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include <optional>
#include <stdexcept>
#include <string>

#include <QMainWindow>
#include <QString>

#include "Core/CPU/Exception.h"

class QMenuBar;

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
  void HandleException(Core::CPU::CPUException e);

  QMenuBar* m_menu_bar;
};
