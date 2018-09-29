// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <QDockWidget>

#include "Common/Types.h"

class CodeViewWidget;
class QListWidget;
class QResizeEvent;
class QSpinBox;

class CodeWidget : public QDockWidget
{
  Q_OBJECT
public:
  explicit CodeWidget();

  void closeEvent(QCloseEvent*) override;

signals:
  void Closed();

private:
  QListWidget* m_stack_list;
  QSpinBox* m_segment_spin;
  QSpinBox* m_offset_spin;

  CodeViewWidget* m_code_view;

  void CreateWidgets();
  void Update();
};
