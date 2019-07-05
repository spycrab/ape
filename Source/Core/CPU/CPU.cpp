// Copyright 2018 Ape Emulator Project
// Licensed under GPLv3+
// Refer to the LICENSE file included.

#include "Core/CPU/CPU.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <type_traits>

#include "Core/CPU/Breakpoint.h"
#include "Core/CPU/Flags.h"
#include "Core/CPU/Instruction.h"
#include "Core/Core.h"
#include "Core/HW/VGA.h"

namespace Core::CPU
{

Type type;

GPR A, B, C, D;

u16& AX = A.X;
u16& BX = B.X;
u16& CX = C.X;
u16& DX = D.X;

u8& AH = A.b8.H;
u8& AL = A.b8.L;

u8& BH = B.b8.H;
u8& BL = B.b8.L;

u8& CH = C.b8.H;
u8& CL = C.b8.L;

u8& DH = D.b8.H;
u8& DL = D.b8.L;

u16 IP = 0;
u16 DI = 0;
u16 SI = 0;
u16 SP = 0;
u16 BP = 0;

u16 CS = 0;
u16 DS = 0;
u16 SS = 0;
u16 ES = 0;

u16 LAST_CS = 0;
u16 LAST_IP = 0;

bool AF = false;
bool CF = false;
bool IF = false;
bool DF = false;
bool OF = false;
bool PF = false;
bool SF = false;
bool ZF = false;

bool simulate_msdos = false;
bool pause_on_boot = false;

std::atomic<bool> running;
std::atomic<bool> paused;

// Treating this as if it were a 5 MHz 8088
u64 clock_speed = 5'000'000;

RepeatMode s_repeat_mode = RepeatMode::None;

void Stop() { running = false; }
void SetPaused(bool value) { paused = value; }

bool IsRunning() { return running; }
bool IsPaused() { return paused; }
State GetState()
{
  if (!running)
    return State::Stopped;

  if (paused)
    return State::Paused;

  return State::Running;
}

std::vector<StateCallbackFunc> fncs;

void RegisterStateChangedCallback(StateCallbackFunc fnc)
{
  fncs.push_back(fnc);
}

template <typename T, typename... U> size_t GetAddress(std::function<T(U...)> f)
{
  typedef T(FunctionType)(U...);
  FunctionType** fn_ptr = f.template target<FunctionType*>();
  return (size_t)*fn_ptr;
}

void UnregisterStateChangedCallback(StateCallbackFunc fnc)
{
  auto it =
      std::find_if(fncs.begin(), fncs.end(), [fnc](const StateCallbackFunc& f) {
        return GetAddress(fnc) == GetAddress(f);
      });

  if (it != fncs.end()) {
    fncs.erase(it);
  }
}

void TriggerCallbacks()
{
  for (auto& fnc : fncs) {
    fnc(GetState());
  }
}

bool HandleRepetition()
{
  if (s_repeat_mode != RepeatMode::None)
    CX--;
  switch (s_repeat_mode) {
  case RepeatMode::Repeat:
    return (CX != 0);
  case RepeatMode::Repeat_Zero:
    return (CX != 0 && ZF);
  case RepeatMode::Repeat_Non_Zero:
    return (CX != 0 && !ZF);
  default:
    return false;
  }
}

Breakpoint just_hit = {0, 0};

void Tick()
{
  const auto old_ip = IP;
  LAST_CS = CS;
  LAST_IP = IP;

  if (IsBreakpointHit() && (just_hit.segment != CS || just_hit.offset != IP)) {
    LOG("Hit a breakpoint at " + String::ToHex(CS) + ":" + String::ToHex(IP) +
        "!");
    SetPaused(true);

    just_hit.segment = CS;
    just_hit.offset = IP;

    return;
  }

  just_hit.segment = 0;
  just_hit.offset = 0;

  u8 opcode = Memory::Get<u8>(CS, IP++);
  Instruction ins(opcode, old_ip);

  if (ins.IsPrefix())
    ins = Instruction(ins, Memory::Get<u8>(CS, IP++), old_ip);

  if (!ins.IsResolved()) {
    u8 mod = Memory::Get<u8>(CS, IP++);
    u8 length = ins.GetLength(mod);

    std::vector<u8> data;

    for (u32 i = 0; i < length; i++)
      data.push_back(Memory::Get<u8>(CS, IP++));

    if (!ins.Resolve(mod, data)) {
      LOG("Failed to resolve " + String::ToHex(opcode) + " with mod " +
          String::ToHex(mod));
      throw InvalidParameterException(opcode, mod);
    }
  }

  if (ins.GetType() == Instruction::Type::Invalid)
    throw InvalidInstructionException(opcode);

  // LOG(String::ToHex<u16>(CS) + ":" + String::ToHex<u16>(old_ip) + ": " +
  //     ins.ToString());

  using Type = Instruction::Type;
  using PType = Instruction::Parameter::Type;

  switch (ins.GetType()) {
  case Type::ADC: {
    ADC(ins);
    break;
  }
  case Type::ADD: {
    ADD(ins);
    break;
  }
  case Type::AND:
    AND(ins);
    break;
  case Type::CALL:
    CALL(ins);
    break;
  case Type::RET:
    RET(ins);
    break;
  case Type::CBW:
    // Copy the sign bit into all of AH
    AH = AL & (0b1000'0000) ? 0xFF : 0x00;
    break;
  case Type::CLC:
    CF = false;
    break;
  case Type::CMC:
    CF = !CF;
    break;
  case Type::STC:
    CF = true;
    break;
  case Type::CLD:
    DF = false;
    break;
  case Type::STD:
    DF = true;
    break;
  case Type::CLI:
    IF = false;
    break;
  case Type::STI:
    IF = true;
    break;
  case Type::CMP: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord()) {
      u16 dst_u16 = ParameterTo<u16>(dst, ins.GetPrefix());
      u16 src_u16;

      i32 diff = static_cast<i16>(dst_u16);

      if (src.IsWord()) {
        src_u16 = ParameterTo<u16>(src, ins.GetPrefix());

        // LOG("Comparing " + String::ToHex<u16>(dst_i16) + " with " +
        //     String::ToHex<u16>(src16));
      } else {
        src_u16 = ParameterTo<u8>(src, ins.GetPrefix());

        // LOG("Comparing " + String::ToHex<u16>(dst_i16) + " with " +
        //     String::ToHex<u8>(src8));
      }

      diff -= static_cast<i16>(src_u16);

      UpdateSF(static_cast<i16>(diff));
      UpdateZF(static_cast<i16>(diff));
      UpdatePF(static_cast<i16>(diff));
      UpdateOF<i16>(diff);
      UpdateCF<i16>(dst_u16 - src_u16);
    } else {
      u8 dst_u8 = ParameterTo<u8>(dst, ins.GetPrefix());
      u8 src_u8 = ParameterTo<u8>(src, ins.GetPrefix());

      i32 diff = static_cast<i8>(dst_u8) - static_cast<i8>(src_u8);

      // LOG("Comparing " + String::ToHex<u8>(dst_i8) + " with " +
      //     String::ToHex<u8>(src_i8));

      UpdateSF(static_cast<i8>(diff));
      UpdateZF(static_cast<i8>(diff));
      UpdatePF(static_cast<i8>(diff));
      UpdateOF<i8>(diff);
      UpdateCF<i8>(dst_u8 - src_u8);
    }

    break;
  }
  case Type::CMPSB:
    CMPSB(ins);
    break;
  case Type::CMPSW:
    CMPSW(ins);
    break;
  case Type::HLT: {
    LOG("[STUB] CPU halted, stopping for now...");
    running = false;
    break;
  }
  case Type::INC:
    INC(ins);
    break;
  case Type::DAA: {
    if ((AL & 0xF) > 9 || AF) {
      bool CF_before = CF;

      UpdateCF<u8>(static_cast<u16>(AL) + 6);
      CF |= CF_before;
      AL += 6;
      AF = true;
    } else {
      AF = false;
    }

    UpdateSF(AL);
    UpdateZF(AL);
    UpdatePF(AL);

    break;
  }
  case Type::DEC:
    DEC(ins);
    break;
  case Type::IDIV: // TODO: This is a bad way to stub IDIV...
  case Type::DIV:
    DIV(ins);
    break;
  case Type::INT: {
    auto& parameter = ins.GetParameters()[0];
    CallInterrupt(parameter.GetData<u8>());
    break;
  }
  case Type::JMP:
    JMP(ins);
    break;
  case Type::JA:
    JA(ins);
    break;
  case Type::JB:
    JB(ins);
    break;
  case Type::JBE:
    JBE(ins);
    break;
  case Type::JNB:
    JNB(ins);
    break;
  case Type::JCXZ:
    JCXZ(ins);
    break;
  case Type::JS:
    JS(ins);
    break;
  case Type::JNS:
    JNS(ins);
    break;
  case Type::JPE:
    JPE(ins);
    break;
  case Type::JPO:
    JPO(ins);
    break;
  case Type::JG:
    JG(ins);
    break;
  case Type::JGE:
    JGE(ins);
    break;
  case Type::JL:
    JL(ins);
    break;
  case Type::JLE:
    JL(ins);
    break;
  case Type::JO:
    JO(ins);
    break;
  case Type::JNO:
    JNO(ins);
    break;
  case Type::JZ:
    JZ(ins);
    break;
  case Type::JNZ:
    JNZ(ins);
    break;
  case Type::LDS: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException(ins, dst, src);

    if (!dst.IsWord())
      throw UnsupportedParameterException(ins, dst);

    u32 ptr;

    ptr = *reinterpret_cast<u32*>(&ParameterTo<u16&>(src, ins.GetPrefix()));

    u16 segment = (ptr & 0xFFFF0000) >> 16;
    u16 offset = ptr & 0xFFFF;

    DS = segment;
    ParameterTo<u16&>(dst, ins.GetPrefix()) = offset;

    break;
  }
  case Type::LEA: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException(ins, dst, src);

    if (!dst.IsWord())
      throw UnsupportedParameterException(ins, dst);

    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());

    dst16 = static_cast<u16>(reinterpret_cast<intptr_t>(&src16) - reinterpret_cast<intptr_t>(Memory::GetPtr<u16>(DS, 0)));

    break;
  }
  case Type::LES: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException(ins, dst, src);

    if (!dst.IsWord())
      throw UnsupportedParameterException(ins, dst);

    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
    u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());

    u32 address = *reinterpret_cast<u32*>(&src16);

    ES = (address & 0xFFFF0000) >> 16;
    dst16 = address & 0xFFFF;
    break;
  }
  case Type::LODSB:
    LODSB(ins);
    break;
  case Type::LODSW:
    LODSW(ins);
    break;
  case Type::LOOP: {
    // LOG("CX = " + String::ToHex(CX));

    CX--;

    if (CX == 0)
      break;

    auto& parameter = ins.GetParameters()[0];

    switch (parameter.GetType()) {
    case PType::Literal_Offset:
      IP += parameter.GetData<i8>();
      break;
    default:
      LOG("[LOOP] Don't know what to do with parameter type: " +
          ParameterTypeToString(parameter.GetType()));
      throw UnhandledParameterException(parameter);
    }
    break;
  }
  case Type::LOOPNZ: {
    CX--;

    if (CX == 0 || !ZF)
      break;

    auto& parameter = ins.GetParameters()[0];

    switch (parameter.GetType()) {
    case PType::Literal_Offset:
      IP += parameter.GetData<i8>();
      break;
    default:
      LOG("[LOOPNZ] Don't know what to do with parameter type: " +
          ParameterTypeToString(parameter.GetType()));
      throw UnhandledParameterException(parameter);
    }
    break;
  }
  case Type::MOV: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException(ins, dst, src);

    if (dst.IsWord()) {
      u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
      u16 src16 = ParameterTo<u16>(src, ins.GetPrefix());

      // LOG("Moving " + String::ToHex(src16) + " -> " + String::ToHex(dst16));

      dst16 = src16;
    } else {
      u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
      u8 src8 = ParameterTo<u8>(src, ins.GetPrefix());

      // LOG("Moving " + String::ToHex(src8) + " -> " + String::ToHex(dst8));

      dst8 = src8;
    }

    break;
  }
  case Type::MOVSB:
    MOVSB(ins);
    break;
  case Type::MOVSW:
    MOVSW(ins);
    break;
  case Type::IMUL: // TODO: This is a bad way to stub IMUL...
  case Type::MUL: {
    auto& mul = ins.GetParameters()[0];

    if (mul.IsWord()) {
      i32 result =
          static_cast<i32>(AX) * ParameterTo<u16>(mul, ins.GetPrefix());

      AX = result & 0xFFFF;
      DX = (result & 0xFFFF0000) >> 16;

      OF = CF = (AX & 0xff00) != 0;
    } else {
      AX = static_cast<i16>(AL) * ParameterTo<u8>(mul, ins.GetPrefix());

      OF = CF = (AX & 0xff00) != 0;
    }

    break;
  }
  case Type::NOP:
    LOG("NOP? NOP.");
    break;
  case Type::OR:
    OR(ins);
    break;
  case Type::PUSH: {
    auto& data = ins.GetParameters()[0];

    if (!data.IsWord())
      throw UnsupportedParameterException(ins, data);

    u16 data16 = ParameterTo<u16>(data, ins.GetPrefix());

    SP -= sizeof(u16);
    Memory::Get<u16>(SS, SP) = data16;

    break;
  }
  case Type::PUSHF: {
    u16 eflags = static_cast<u16>(CF) | (1 << 1) | (static_cast<u16>(PF) << 2) |
                 (static_cast<u16>(AF) << 4) | (static_cast<u16>(ZF) << 6) |
                 (static_cast<u16>(SF) << 7) | (/*TF*/ 0 << 8) |
                 (static_cast<u16>(IF) << 9) | (static_cast<u16>(DF) << 10) |
                 (static_cast<u16>(OF) << 11) | (1 << 14) | (1 << 15);

    SP -= sizeof(u16);
    Memory::Get<u16>(SS, SP) = eflags;

    break;
  }
  case Type::POPF: {
    u16 eflags = Memory::Get<u16>(SS, SP);

    CF = eflags & 1;
    PF = eflags & (1 << 2);
    AF = eflags & (1 << 4);
    ZF = eflags & (1 << 6);
    SF = eflags & (1 << 7);
    // TF = eflags & (1 << 8);
    IF = eflags & (1 << 9);
    DF = eflags & (1 << 10);
    OF = eflags & (1 << 11);

    SP -= sizeof(u16);

    break;
  }
  case Type::POP: {
    auto& dst = ins.GetParameters()[0];

    if (!dst.IsWord())
      throw UnsupportedParameterException(ins, dst);

    u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());

    dst16 = Memory::Get<u16>(SS, SP);
    SP += sizeof(u16);

    break;
  }
  case Type::REPZ:
    s_repeat_mode = RepeatMode::Repeat_Zero;
    return;
  case Type::REPNZ:
    s_repeat_mode = RepeatMode::Repeat_Non_Zero;
    return;
  case Type::ROL:
    ROL(ins);
    break;
  case Type::ROR:
    ROR(ins);
    break;
  case Type::SHL:
    SHL(ins);
    break;
  case Type::SHR:
    SHR(ins);
    break;
  case Type::STOSB:
    STOSB(ins);
    break;
  case Type::STOSW:
    STOSW(ins);
    break;
  case Type::SBB:
    SBB(ins);
    break;
  case Type::SUB:
    SUB(ins);
    break;
  case Type::TEST:
    TEST(ins);
    break;
  case Type::XCHG: {
    auto& dst = ins.GetParameters()[0];
    auto& src = ins.GetParameters()[1];

    if (dst.IsWord() != src.IsWord())
      throw ParameterLengthMismatchException(ins, dst, src);

    if (dst.IsWord()) {
      u16& dst16 = ParameterTo<u16&>(dst, ins.GetPrefix());
      u16& src16 = ParameterTo<u16&>(src, ins.GetPrefix());
      std::swap(dst16, src16);
    } else {
      u8& dst8 = ParameterTo<u8&>(dst, ins.GetPrefix());
      u8& src8 = ParameterTo<u8&>(src, ins.GetPrefix());
      std::swap(dst8, src8);
    }

    break;
  }
  case Type::XOR:
    XOR(ins);
    break;
  default:
    throw UnhandledInstructionException(ins);
  }

  s_repeat_mode = RepeatMode::None;
}

void Start()
{
  running = true;
  TriggerCallbacks();
  u8 counter = 0;

  if (pause_on_boot)
    paused = true;

  while (running) {
    if (counter++ == 0)
      Core::HW::VGA::Update();

    if (paused)
      TriggerCallbacks();

    while (paused && running) {
    }

    Tick();

    std::this_thread::sleep_for(
        std::chrono::nanoseconds(1000000000 / clock_speed));
  }

  TriggerCallbacks();

  // Update the output for the last time before stopping so all output gets
  // shown
  Core::HW::VGA::Update();
}

u16 PrefixToValue(Instruction::SegmentPrefix prefix)
{
  using Prefix = Instruction::SegmentPrefix;
  switch (prefix) {
  case Prefix::CS:
    return CS;
  case Prefix::ES:
    return ES;
  case Prefix::SS:
    return SS;

  case Prefix::DS:
  default:
    return DS;
  }
}
} // namespace Core::CPU
