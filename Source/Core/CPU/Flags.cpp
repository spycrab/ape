#include "Core/CPU/CPU.h"

using namespace Core;

void CPU::UpdateZF(u16 value) { ZF = (value == 0); }

void CPU::UpdatePF(u16 value) { PF = (value % 2 == 0); }

void CPU::UpdateSF(i16 value) { SF = value < 0; }
