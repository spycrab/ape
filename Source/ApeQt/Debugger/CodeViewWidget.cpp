// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/Debugger/CodeViewWidget.h"

#include <cmath>

#include <QHeaderView>
#include <QMenu>
#include <QTableWidgetItem>

#include "ApeQt/QueueOnObject.h"

#include "Core/CPU/Breakpoint.h"
#include "Core/CPU/CPU.h"

CodeViewWidget::CodeViewWidget()
{

  setColumnCount(3);

  horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

  setShowGrid(false);
  verticalHeader()->hide();
  horizontalHeader()->hide();
  horizontalHeader()->setStretchLastSection(true);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QTableWidget::customContextMenuRequested, this,
          &CodeViewWidget::OnContextMenu);

  Core::CPU::RegisterStateChangedCallback([this](Core::CPU::State) {
    m_segment = Core::CPU::CS;
    m_offset = Core::CPU::IP;
    QueueOnObject(this, [this] { Update(); });
  });

  Update();
}

void CodeViewWidget::Update()
{
  clear();
  setRowCount(1);

  int rows = std::round((height() / static_cast<float>(rowHeight(0))));

  setRowCount(rows);

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

      std::vector<u8> ins_data;

      for (u16 j = 0; j < length; j++) {
        ins_data.push_back(Core::Memory::Get<u8>(segment, offset++));
      }

      ins.Resolve(mod, ins_data);
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

    setItem(i, 0, segment_item);
    setItem(i, 1, offset_item);
    setItem(i, 2, disas_item);
  }
}

u16 CodeViewWidget::GetSegment() const { return m_segment; }
u16 CodeViewWidget::GetOffset() const { return m_offset; }

void CodeViewWidget::SetSegment(u16 segment) { m_segment = segment; }
void CodeViewWidget::SetOffset(u16 offset) { m_offset = offset; }

void CodeViewWidget::OnContextMenu()
{
  auto items = selectedItems();

  if (items.isEmpty())
    return;

  auto item = selectedItems()[0];

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
    Core::CPU::CS = m_segment;
    Core::CPU::IP = m_offset;
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

  menu->addAction(tr("Update"), this, &CodeViewWidget::Update);

  menu->exec(QCursor::pos());
  Update();
}

void CodeViewWidget::resizeEvent(QResizeEvent*) { Update(); }
