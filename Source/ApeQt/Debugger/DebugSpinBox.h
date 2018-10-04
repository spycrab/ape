// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <QSpinBox>

class DebugSpinBox : public QSpinBox
{
  Q_OBJECT
public:
  DebugSpinBox(int size);

  QString textFromValue(int value) const override;
  QSize sizeHint() const override;
};