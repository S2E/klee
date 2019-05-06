//===-- Time.cpp ----------------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Internal/System/Time.h"

#include "klee/Config/config.h"

#include "llvm/Support/Chrono.h"
#include "llvm/Support/Process.h"

using namespace llvm;
using namespace klee;

static double toSeconds(const std::chrono::nanoseconds &duration) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / (double) 1000000000;
}

double util::getUserTime() {
    llvm::sys::TimePoint<> now;
    std::chrono::nanoseconds user, sys;
    sys::Process::GetTimeUsage(now, user, sys);
    return toSeconds(user);
}

double util::getWallTime() {
    llvm::sys::TimePoint<> now;
    std::chrono::nanoseconds user, sys;
    sys::Process::GetTimeUsage(now, user, sys);
    return toSeconds(now.time_since_epoch());
}
