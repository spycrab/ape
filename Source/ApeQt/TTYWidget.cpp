// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/TTYWidget.h"
#include "ApeQt/QueueOnObject.h"

#include <QString>

#include "Core/TTY.h"

TTYWidget::TTYWidget() { g_TTYBackend = this; }
TTYWidget::~TTYWidget() { g_TTYBackend = nullptr; }

void TTYWidget::Write(const std::string& string)
{
  QueueOnObject(this, [this, string] {
    insertPlainText(QString::fromStdString(string));
  });
}

void TTYWidget::Write(const char c)
{
  QueueOnObject(this, [this, c] { insertPlainText(QString(QLatin1Char(c))); });
}

void TTYWidget::Clear() { clear(); }

char TTYWidget::Read() { return 'x'; }

void TTYWidget::Scroll(const u8, const u8) {}
void TTYWidget::MoveCursor(const u32, const u32) {}
