//===-- Timer.cpp ---------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Internal/Support/Timer.h"
#include "klee/Internal/System/Time.h"

#include "klee/Config/config.h"

#include "llvm/Support/Chrono.h"
#include "llvm/Support/Process.h"

using namespace klee;
using namespace llvm;

WallTimer::WallTimer() {
    llvm::sys::TimePoint<> now;
    std::chrono::nanoseconds user, sys;
    sys::Process::GetTimeUsage(now, user, sys);
    m_start = now.time_since_epoch();
}

uint64_t WallTimer::check() {
    llvm::sys::TimePoint<> now;
    std::chrono::nanoseconds user, sys;
    sys::Process::GetTimeUsage(now, user, sys);

    auto diff = now.time_since_epoch() - m_start;
    return std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
}
