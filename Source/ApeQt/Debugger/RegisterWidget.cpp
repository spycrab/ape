// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "ApeQt/Debugger/RegisterWidget.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QWidget>

#include "ApeQt/QueueOnObject.h"
#include "ApeQt/Debugger/DebugSpinBox.h"

#include "Core/CPU/CPU.h"

RegisterWidget::RegisterWidget()
{
  setWindowTitle(tr("Registers"));
  CreateWidgets();

  setVisible(QSettings().value("debug/showregister", true).toBool());

  Core::CPU::RegisterStateChangedCallback(
      [this](Core::CPU::State) { QueueOnObject(this, [this] { Update(); }); });
}

void RegisterWidget::Update() { emit OnUpdate(); }

void RegisterWidget::closeEvent(QCloseEvent*)
{
  QSettings().setValue("debug/showregister", false);
  emit Closed();
}

QSpinBox* RegisterWidget::Get16BitInput(u16* value)
{
  auto* spin = new DebugSpinBox(sizeof(u16));
  spin->setValue(*value);

  connect(this, &RegisterWidget::OnUpdate, this, [spin, value] {
    QSignalBlocker blocker(spin);
    spin->setValue(*value);
  });
  connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [spin, value] { *value = spin->value(); });

  return spin;
}

QSpinBox* RegisterWidget::Get8BitInput(u8* value)
{
  auto* spin = new DebugSpinBox(sizeof(u8));

  spin->setValue(*value);

  connect(this, &RegisterWidget::OnUpdate, this, [spin, value] {
    QSignalBlocker blocker(spin);
    spin->setValue(*value);
  });
  connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [spin, value] { *value = spin->value(); });

  return spin;
}

QCheckBox* RegisterWidget::GetFlagInput(QString label, bool* value)
{
  auto* check = new QCheckBox(label);

  check->setChecked(*value);

  connect(this, &RegisterWidget::OnUpdate, this, [check, value] {
    QSignalBlocker blocker(check);
    check->setChecked(*value);
  });
  connect(check, &QCheckBox::toggled, this,
          [check, value] { *value = check->isChecked(); });

  return check;
}

void RegisterWidget::CreateWidgets()
{
  auto* layout = new QVBoxLayout;

  auto* gp_box = new QGroupBox(tr("General Purpose"));
  auto* gp_layout = new QGridLayout;

  gp_box->setLayout(gp_layout);

  gp_layout->addWidget(new QLabel("AX"), 0, 0);
  gp_layout->addWidget(Get16BitInput(&Core::CPU::AX), 0, 1);
  gp_layout->addWidget(new QLabel("AH"), 0, 2);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::AH), 0, 3);
  gp_layout->addWidget(new QLabel("AL"), 0, 4);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::AL), 0, 5);

  gp_layout->addWidget(new QLabel("BX"), 1, 0);
  gp_layout->addWidget(Get16BitInput(&Core::CPU::BX), 1, 1);
  gp_layout->addWidget(new QLabel("BH"), 1, 2);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::BH), 1, 3);
  gp_layout->addWidget(new QLabel("BL"), 1, 4);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::BL), 1, 5);

  gp_layout->addWidget(new QLabel("CX"), 2, 0);
  gp_layout->addWidget(Get16BitInput(&Core::CPU::CX), 2, 1);
  gp_layout->addWidget(new QLabel("CH"), 2, 2);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::CH), 2, 3);
  gp_layout->addWidget(new QLabel("CL"), 2, 4);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::CL), 2, 5);

  gp_layout->addWidget(new QLabel("DX"), 3, 0);
  gp_layout->addWidget(Get16BitInput(&Core::CPU::DX), 3, 1);
  gp_layout->addWidget(new QLabel("DH"), 3, 2);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::DH), 3, 3);
  gp_layout->addWidget(new QLabel("DL"), 3, 4);
  gp_layout->addWidget(Get8BitInput(&Core::CPU::DL), 3, 5);

  gp_layout->setColumnStretch(1, 1);
  gp_layout->setColumnStretch(3, 1);
  gp_layout->setColumnStretch(5, 1);

  auto* flags_box = new QGroupBox(tr("Flags"));
  auto* flags_layout = new QGridLayout;

  flags_box->setLayout(flags_layout);

  flags_layout->addWidget(GetFlagInput("OF", &Core::CPU::OF), 0, 0);
  flags_layout->addWidget(GetFlagInput("DF", &Core::CPU::DF), 0, 1);
  flags_layout->addWidget(GetFlagInput("IF", &Core::CPU::IF), 0, 2);
  flags_layout->addWidget(GetFlagInput("SF", &Core::CPU::SF), 0, 3);

  flags_layout->addWidget(GetFlagInput("ZF", &Core::CPU::ZF), 1, 0);
  flags_layout->addWidget(GetFlagInput("AF", &Core::CPU::AF), 1, 1);
  flags_layout->addWidget(GetFlagInput("PF", &Core::CPU::PF), 1, 2);
  flags_layout->addWidget(GetFlagInput("CF", &Core::CPU::CF), 1, 3);

  auto* si_box = new QGroupBox(tr("Segment /  Index Registers"));
  auto* si_layout = new QGridLayout;

  si_box->setLayout(si_layout);

  si_layout->addWidget(new QLabel("CS"), 0, 0);
  si_layout->addWidget(Get16BitInput(&Core::CPU::CS), 0, 1);
  si_layout->addWidget(new QLabel("IP"), 0, 2);
  si_layout->addWidget(Get16BitInput(&Core::CPU::IP), 0, 3);

  si_layout->addWidget(new QLabel("SS"), 1, 0);
  si_layout->addWidget(Get16BitInput(&Core::CPU::SS), 1, 1);
  si_layout->addWidget(new QLabel("SP"), 1, 2);
  si_layout->addWidget(Get16BitInput(&Core::CPU::SP), 1, 3);

  si_layout->addWidget(new QLabel("DS"), 2, 0);
  si_layout->addWidget(Get16BitInput(&Core::CPU::DS), 2, 1);
  si_layout->addWidget(new QLabel("SI"), 2, 2);
  si_layout->addWidget(Get16BitInput(&Core::CPU::SI), 2, 3);

  si_layout->addWidget(new QLabel("ES"), 3, 0);
  si_layout->addWidget(Get16BitInput(&Core::CPU::ES), 3, 1);
  si_layout->addWidget(new QLabel("DI"), 3, 2);
  si_layout->addWidget(Get16BitInput(&Core::CPU::DI), 3, 3);

  si_layout->setColumnStretch(1, 1);
  si_layout->setColumnStretch(3, 1);

  layout->addWidget(gp_box);
  layout->addWidget(flags_box);
  layout->addWidget(si_box);
  layout->addStretch();

  auto* widget = new QWidget;

  widget->setLayout(layout);

  setWidget(widget);
}
