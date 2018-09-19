// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <QTextBrowser>

#include "Core/HW/VGA.h"

class TTYWidget : public QTextBrowser, public Core::HW::VGABackend
{
public:
  explicit TTYWidget(Core::HW::VGACard* card);
  ~TTYWidget();

  void SetMode(u8 mode) override;
  void Update() override;
};
