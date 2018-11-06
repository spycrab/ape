// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/HW/VGA.h"

#include "Common/Logger.h"
#include "Common/String.h"

#include "Core/Core.h"
#include "Core/Memory.h"

Core::HW::VGABackend* g_VGABackend = nullptr;

using namespace Core::HW;

void VGA::Init()
{
  for (size_t y = 0; y < 25; y++)
    for (size_t x = 0; x < 80; x++) {
      GetBuffer()[(y * 80 + x) * sizeof(u16)] = ' ';
      GetBuffer()[(y * 80 + x) * sizeof(u16) + 1] = 0x0F;
    }
}

bool VGA::IsPresent() { return g_VGABackend != nullptr; }

void VGA::SetMode(u8 mode)
{
  if (IsPresent())
    g_VGABackend->SetMode(mode);
}

void VGA::Update()
{
  if (IsPresent())
    g_VGABackend->Update();
}

u8* VGA::GetBuffer() { return Memory::GetPtr<u8>(0xB000, 0x8000); }
