// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include "Common/Types.h"

namespace Core::HW
{
namespace VGA
{
void Init();
void Update();

bool IsPresent();

void SetMode(u8 mode);
u8* GetBuffer();
} // namespace VGA

class VGABackend
{
public:
  virtual void SetMode(u8 mode) = 0;
  virtual void Update() = 0;
};
} // namespace Core::HW

extern Core::HW::VGABackend* g_VGABackend;
