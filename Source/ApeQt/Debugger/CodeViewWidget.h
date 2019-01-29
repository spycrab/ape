// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <QTableWidget>

#include "Common/Types.h"

class CodeViewWidget : public QTableWidget
{
  Q_OBJECT
public:
  CodeViewWidget();

  u16 GetSegment() const;
  u16 GetOffset() const;

  void SetSegment(u16 segment);
  void SetOffset(u16 offset);

  void Update();

  void resizeEvent(QResizeEvent*) override;

private:
  void OnContextMenu();

  u16 m_segment = 0, m_offset = 0;
};
