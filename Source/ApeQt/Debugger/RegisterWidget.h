// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <QDockWidget>
#include <QString>

#include "Common/Types.h"

class QCheckBox;
class QSpinBox;

class RegisterWidget : public QDockWidget
{
  Q_OBJECT
public:
  explicit RegisterWidget();
  void Update();

signals:
  void OnUpdate();

private:
  void CreateWidgets();

  QSpinBox* Get16BitInput(u16* value);
  QSpinBox* Get8BitInput(u8* value);
  QCheckBox* GetFlagInput(QString label, bool* value);
};
