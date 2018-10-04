// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/Debugger/CodeWidget.h"

#include <cmath>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include "ApeQt/Debugger/CodeViewWidget.h"
#include "ApeQt/QueueOnObject.h"
#include "ApeQt/Debugger/DebugSpinBox.h"

#include "Core/CPU/Breakpoint.h"
#include "Core/CPU/CPU.h"
#include "Core/CPU/Instruction.h"
#include "Core/Memory.h"

CodeWidget::CodeWidget()
{
  setWindowTitle(tr("Code"));
  CreateWidgets();

  setVisible(QSettings().value("debug/showcode", true).toBool());

  Core::CPU::RegisterStateChangedCallback(
      [this](Core::CPU::State) { QueueOnObject(this, [this] { Update(); }); });

  Update();
}

void CodeWidget::CreateWidgets()
{
  auto* jump_layout = new QHBoxLayout;

  m_segment_spin = new DebugSpinBox(sizeof(u16));
  m_offset_spin = new DebugSpinBox(sizeof(u16));
  m_code_view = new CodeViewWidget;

  auto* scroll_btn = new QPushButton(tr("Scroll"));

  connect(scroll_btn, &QPushButton::pressed, this, [this] {
    m_code_view->SetSegment(m_segment_spin->value());
    m_code_view->SetOffset(m_offset_spin->value());
    Update();
  });

  jump_layout->addStretch();
  jump_layout->addWidget(m_segment_spin);
  jump_layout->addWidget(new QLabel(":"));
  jump_layout->addWidget(m_offset_spin);
  jump_layout->addWidget(scroll_btn);
  jump_layout->addStretch();

  auto* stack_box = new QGroupBox(tr("Stack"));
  auto* stack_layout = new QVBoxLayout;

  stack_box->setLayout(stack_layout);

  m_stack_list = new QListWidget;

  stack_layout->addWidget(m_stack_list);

  auto* splitter = new QSplitter(Qt::Vertical);
  auto* widget = new QWidget;
  auto* layout = new QVBoxLayout;

  widget->setLayout(layout);

  splitter->addWidget(m_code_view);
  splitter->addWidget(stack_box);

  layout->addLayout(jump_layout);
  layout->addWidget(splitter);

  setWidget(widget);
}

void CodeWidget::closeEvent(QCloseEvent*)
{
  QSettings().setValue("debug/showcode", false);
  emit Closed();
}

void CodeWidget::Update()
{
  m_segment_spin->setValue(m_code_view->GetSegment());
  m_offset_spin->setValue(m_code_view->GetOffset());

  m_stack_list->clear();

  for (u32 i = 0; i < 16; i++) {
    m_stack_list->addItem(new QListWidgetItem(
        QStringLiteral("%1:%2 %3")
            .arg(Core::CPU::SS, 4, 16, QLatin1Char('0'))
            .arg(Core::CPU::SP + i * sizeof(u16), 4, 16, QLatin1Char('0'))
            .arg(Core::Memory::Get<u16>(
                     Core::CPU::SS,
                     static_cast<u16>(Core::CPU::SP + i * sizeof(u16))),
                 4, 16, QLatin1Char('0'))));
  }
}
