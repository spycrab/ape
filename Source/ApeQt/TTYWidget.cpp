// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/TTYWidget.h"
#include "ApeQt/QueueOnObject.h"

#include <QFont>
#include <QKeyEvent>
#include <QString>
#include <QTextBlock>
#include <QVector>

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/TTY.h"

TTYWidget::TTYWidget(Core::HW::VGACard* card) : VGABackend(card)
{
  g_VGABackend = this;

  QFont font("Monospace");
  font.setStyleHint(QFont::TypeWriter);
  setFont(font);
}
TTYWidget::~TTYWidget() { g_VGABackend = nullptr; }

void TTYWidget::SetMode(u8 mode)
{
  LOG("Ignoring mode set to " + String::ToHex(mode) +
      ". TTYWidget only supports text mode!");
}

void TTYWidget::Update()
{
  static const QVector<QString> color_str = {
      "black",      "blue",         "green",      "cyan",
      "red",        "magenta",      "brown",      "lightgray",
      "lightblack", "lightblue",    "lightgreen", "lightcyan",
      "lightred",   "lightmagenta", "yellow",     "white"};

  u8* buffer = GetCard()->GetBuffer();

  constexpr int COLUMNS = 80;
  constexpr int ROWS = 25;

  QString s(ROWS * (COLUMNS + 1));

  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      char c = buffer[(y * COLUMNS + x) * sizeof(u16)];
      s[y * (COLUMNS + 1) + x] = std::isprint(c) ? c : ' ';
    }
    s[(y + 1) * (COLUMNS + 1) - 1] = '\n';
  }

  QueueOnObject(this, [this, s] {
    clear();
    append(s);
  });
}
