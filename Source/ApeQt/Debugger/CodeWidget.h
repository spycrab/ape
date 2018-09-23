// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <QDockWidget>

#include "Common/Types.h"

class QListWidget;
class QTableWidget;
class QResizeEvent;
class QSpinBox;

class CodeWidget : public QDockWidget
{
  Q_OBJECT
public:
  explicit CodeWidget();

  void resizeEvent(QResizeEvent*);

private:
  QListWidget* m_stack_list;
  QTableWidget* m_table;
  QSpinBox* m_segment_spin;
  QSpinBox* m_offset_spin;

  void CreateWidgets();
  void Update();

  void OnContextMenu();

  u16 m_segment = 0, m_offset = 0;
};
