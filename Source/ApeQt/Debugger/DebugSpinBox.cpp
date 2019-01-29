// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/Debugger/DebugSpinBox.h"

#include <cmath>

#include <QFontMetrics>

DebugSpinBox::DebugSpinBox(int size)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setDisplayIntegerBase(16);
  setMinimum(0);
  setMaximum(std::pow(2, size * 8) - 1);
}

QString DebugSpinBox::textFromValue(int value) const
{
  return QString("%1").arg(value, maximum() > 0xff ? 4 : 2, 16, QChar('0'));
}

QSize DebugSpinBox::sizeHint() const
{
  QSize hint = QSpinBox::sizeHint();
  QFontMetrics m(font());

  hint.setWidth(m.width("ffff"));

  return hint;
}
