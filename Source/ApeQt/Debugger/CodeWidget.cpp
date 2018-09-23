// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/Debugger/CodeWidget.h"

#include <cmath>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "Core/CPU/Breakpoint.h"
#include "Core/CPU/CPU.h"
#include "Core/CPU/Instruction.h"
#include "Core/Memory.h"

CodeWidget::CodeWidget()
{
  setWindowTitle(tr("Code"));
  CreateWidgets();

  m_table->setColumnCount(3);

  m_table->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  m_table->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);

  m_table->setShowGrid(false);
  m_table->verticalHeader()->hide();
  m_table->horizontalHeader()->hide();
  m_table->horizontalHeader()->setStretchLastSection(true);

  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_table->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(m_table, &QTableWidget::customContextMenuRequested, this,
          &CodeWidget::OnContextMenu);

  Core::CPU::RegisterStateChangedCallback([this](Core::CPU::State) {
    m_segment = Core::CPU::CS;
    m_offset = Core::CPU::IP;
    Update();
  });

  Update();
}

void CodeWidget::CreateWidgets()
{
  auto* jump_layout = new QHBoxLayout;

  m_segment_spin = new QSpinBox;
  m_offset_spin = new QSpinBox;

  for (auto* spin : {m_segment_spin, m_offset_spin}) {
    spin->setDisplayIntegerBase(16);
    spin->setMinimum(0);
    spin->setMaximum(0xffff);
  }

  auto* scroll_btn = new QPushButton(tr("Scroll"));

  connect(scroll_btn, &QPushButton::pressed, this, [this] {
    m_segment = m_segment_spin->value();
    m_offset = m_offset_spin->value();
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

  m_table = new QTableWidget;

  auto* splitter = new QSplitter(Qt::Vertical);
  auto* widget = new QWidget;
  auto* layout = new QVBoxLayout;

  widget->setLayout(layout);

  splitter->addWidget(m_table);
  splitter->addWidget(stack_box);

  layout->addLayout(jump_layout);
  layout->addWidget(splitter);

  setWidget(widget);
}

void CodeWidget::Update()
{
  m_segment_spin->setValue(m_segment);
  m_offset_spin->setValue(m_offset);

  m_stack_list->clear();

  for (u32 i = 0; i < 16; i++) {
    m_stack_list->addItem(new QListWidgetItem(
        QStringLiteral("%1:%2 %3")
            .arg(Core::CPU::SS, 4, 16, QLatin1Char('0'))
            .arg(Core::CPU::SP + i * sizeof(u16), 4, 16, QLatin1Char('0'))
            .arg(Core::Memory::Get<u16>(Core::CPU::SS,
                                        Core::CPU::SP + i * sizeof(u16)),
                 4, 16, QLatin1Char('0'))));
  }

  m_table->clear();
  m_table->setRowCount(1);

  int rows = std::round(
      (m_table->height() / static_cast<float>(m_table->rowHeight(0))));

  m_table->setRowCount(rows);

  u16 segment = m_segment;
  u16 offset = m_offset;

  for (int i = 0; i < rows; i++) {
    u16 base = offset;
    auto ins =
        Core::CPU::Instruction(Core::Memory::Get<u8>(segment, offset++), base);

    QString ins_str = tr("Unresolved");

    if (ins.IsPrefix())
      ins = Core::CPU::Instruction(
          ins, Core::Memory::Get<u8>(segment, offset++), base);

    if (!ins.IsResolved()) {
      u8 mod = static_cast<u8>(Core::Memory::Get<u8>(segment, offset++));
      u8 length = ins.GetLength(mod);

      std::vector<u8> data;

      for (u16 i = 0; i < length; i++) {
        data.push_back(Core::Memory::Get<u8>(segment, offset++));
      }

      ins.Resolve(mod, data);
    }

    ins_str = QString::fromStdString(ins.ToString());

    auto* segment_item = new QTableWidgetItem(
        QStringLiteral("%1").arg(segment, 4, 16, QLatin1Char('0')));

    auto* offset_item = new QTableWidgetItem(
        QStringLiteral("%1").arg(base, 4, 16, QLatin1Char('0')));

    auto* disas_item = new QTableWidgetItem(ins_str);

    for (auto* item : {segment_item, offset_item, disas_item}) {
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      item->setData(Qt::UserRole, segment);
      item->setData(Qt::UserRole + 1, base);
      item->setData(Qt::UserRole + 2, offset - base);

      if (Core::CPU::IsBreakpoint(segment, base))
        item->setBackground(Qt::red);

      QFont bold;
      bold.setBold(true);

      if (segment == Core::CPU::CS && base == Core::CPU::IP)
        item->setFont(bold);
    }

    m_table->setItem(i, 0, segment_item);
    m_table->setItem(i, 1, offset_item);
    m_table->setItem(i, 2, disas_item);
  }
}

void CodeWidget::resizeEvent(QResizeEvent*) { Update(); }

void CodeWidget::OnContextMenu()
{
  auto items = m_table->selectedItems();

  if (items.isEmpty())
    return;

  auto item = m_table->selectedItems()[0];

  u16 segment = item->data(Qt::UserRole).toInt();
  u16 offset = item->data(Qt::UserRole + 1).toInt();
  u16 length = item->data(Qt::UserRole + 2).toInt();

  auto* menu = new QMenu(this);

  menu->addAction(tr("Scroll To Here"), this, [this, segment, offset] {
    m_segment = segment;
    m_offset = offset;
  });

  menu->addAction(tr("Scroll To CS:IP"), this, [this] {
    m_segment = Core::CPU::CS;
    m_offset = Core::CPU::IP;
  });

  menu->addSeparator();

  menu->addAction(tr("Jump To Here"), this, [this, segment, offset] {
    m_segment = Core::CPU::CS;
    m_offset = Core::CPU::IP;
  });

  menu->addSeparator();

  menu->addAction(tr("Replace with NOP"), this,
                  [this, segment, offset, length] {
                    for (u16 i = 0; i < length; i++)
                      Core::Memory::Get<u8>(segment, offset + i) = 0x90;
                  });

  menu->addAction(tr("Toggle breakpoint"), this, [this, segment, offset] {
    Core::CPU::Breakpoint bp{segment, offset};

    if (Core::CPU::IsBreakpoint(segment, offset))
      Core::CPU::RemoveBreakpoint(bp);
    else
      Core::CPU::AddBreakpoint(bp);
  });

  menu->addSeparator();

  menu->addAction(tr("Update"), this, &CodeWidget::Update);

  menu->exec(QCursor::pos());
  Update();
}
