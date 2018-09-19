// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#pragma once

#include "Common/Types.h"

namespace Core
{
class Machine;
}

namespace Core::HW
{
class VGACard
{
public:
  VGACard(Core::Machine* machine);
  void Update();

  void SetMode(u8 mode);
  u8* GetBuffer();

private:
  u8 mode;
  Core::Machine* m_machine;
}; // namespace VGA

class VGABackend
{
public:
  VGABackend(VGACard* card);
  virtual void SetMode(u8 mode) = 0;
  virtual void Update() = 0;

protected:
  VGACard* GetCard();

private:
  VGACard* m_card;
};
} // namespace Core::HW

extern Core::HW::VGABackend* g_VGABackend;
