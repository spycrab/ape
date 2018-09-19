// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/HW/VGA.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/Machine.h"

Core::HW::VGABackend* g_VGABackend = nullptr;

using namespace Core::HW;

VGACard::VGACard(Core::Machine* machine) : m_machine(machine) {}

void VGACard::SetMode(u8 mode)
{
  if (g_VGABackend != nullptr)
    g_VGABackend->SetMode(mode);
}

void VGACard::Update()
{
  if (g_VGABackend != nullptr)
    g_VGABackend->Update();
}

u8* VGACard::GetBuffer()
{
  return m_machine->GetMemory().GetPtr<u8>(0xB000, 0x8000);
}

VGABackend::VGABackend(VGACard* card) : m_card(card) {}

VGACard* VGABackend::GetCard() { return m_card; }
