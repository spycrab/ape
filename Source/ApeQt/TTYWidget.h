// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include <map>

#include <QTextBrowser>

#include "Core/TTYBackend.h"

class QKeyEvent;

class TTYWidget : public QTextBrowser, public TTYBackend
{
public:
  explicit TTYWidget();
  ~TTYWidget();

  void Write(const std::string& string) override;
  void Write(const char c) override;

  void Scroll(const u8 lines, const u8 colors) override;
  void MoveCursor(const u32 x, const u32 y) override;

  u8 GetCursorRow() const override;
  void SetCursorRow(u8 row) override;

  u8 GetCursorColumn() const override;
  void SetCursorColumn(u8 column) override;

  void Clear() override;
  char Read() override;
  bool IsCharAvailable() const override;

  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);

  u8 m_row = 0, m_column = 0;
  std::map<char, bool> m_pressed_keys;
};
