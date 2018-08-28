// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/TTYWidget.h"
#include "ApeQt/QueueOnObject.h"

#include <QKeyEvent>
#include <QString>
#include <QTextBlock>

#include "Common/Logger.h"

#include "Core/TTY.h"

TTYWidget::TTYWidget()
{
  g_TTYBackend = this;
  insertPlainText(QStringLiteral("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
  auto cursor = textCursor();
  cursor.setPosition(0);
  setTextCursor(cursor);
}
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

char TTYWidget::Read()
{
  while (m_pressed_keys.empty()) {
  }

  return (*m_pressed_keys.begin()).first;
}

bool TTYWidget::IsCharAvailable() const { return !m_pressed_keys.empty(); }

void TTYWidget::Scroll(const u8, const u8) {}
void TTYWidget::MoveCursor(const u32 row, const u32 column)
{
  m_row = row;
  m_column = column;
}

u8 TTYWidget::GetCursorRow() const { return m_row; }

void TTYWidget::SetCursorRow(u8 row)
{
  if (m_row == row)
    return;

  m_row = row;

  QueueOnObject(this, [this, row] {
    auto cursor = textCursor();
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row);
    setTextCursor(cursor);

  });
}

u8 TTYWidget::GetCursorColumn() const { return m_column; }
void TTYWidget::SetCursorColumn(u8 column)
{
  if (m_column == column)
    return;

  m_column = column;

  QueueOnObject(this, [this, column] {
    auto cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
    setTextCursor(cursor);
  });
}

void TTYWidget::keyPressEvent(QKeyEvent* event)
{
  std::string s = event->text().toStdString();
  char c = s[0];

  m_pressed_keys[c] = true;
}

void TTYWidget::keyReleaseEvent(QKeyEvent* event)
{
  char c = event->text().toStdString()[0];

  if (m_pressed_keys.count(c))
    m_pressed_keys.erase(m_pressed_keys.find(c));
}
