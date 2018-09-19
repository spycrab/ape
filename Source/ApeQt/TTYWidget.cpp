// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/TTYWidget.h"
#include "ApeQt/QueueOnObject.h"

#include <QKeyEvent>
#include <QString>
#include <QTextBlock>

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/TTY.h"

TTYWidget::TTYWidget(Core::HW::VGACard* card) : VGABackend(card)
{
  g_VGABackend = this;
}
TTYWidget::~TTYWidget() { g_VGABackend = nullptr; }

void TTYWidget::SetMode(u8 mode)
{
  LOG("Ignoring mode set to " + String::ToHex(mode) +
      ". TTYWidget only supports text mode!");
}

void TTYWidget::Update()
{
  u8* buffer = GetCard()->GetBuffer();

  constexpr u64 COLUMNS = 80;
  constexpr u64 ROWS = 25;

  QString s;

  for (u64 y = 0; y < ROWS; y++) {
    for (u64 x = 0; x < COLUMNS; x++) {
      char c = buffer[(y * COLUMNS + x) * sizeof(u16)];

      s += QChar(c);
    }

    s += "\n";
  }

  QueueOnObject(this, [this, s] {
    clear();
    append(s);
  });
}
