//===-- ExternalDispatcher.cpp --------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <klee/ExternalDispatcher.h>
#include <klee/Common.h>
#include <llvm/Support/DynamicLibrary.h>

#include <iostream>
#include <sstream>

#include <setjmp.h>
#include <signal.h>
#include <dlfcn.h>

namespace klee {
ExternalDispatcher::ExternalDispatcher() {
}

ExternalDispatcher::~ExternalDispatcher() {
}

void *ExternalDispatcher::resolveSymbol(const std::string &name) {
    const char *str = name.c_str();

    // We use this to validate that function names can be resolved so we
    // need to match how the JIT does it. Unfortunately we can't
    // directly access the JIT resolution function
    // JIT::getPointerToNamedFunction so we emulate the important points.

    if (str[0] == 1) { // asm specifier, skipped
        ++str;
    }

    void *addr = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(str);
    if (addr) {
        return addr;
    }
    // If it has an asm specifier and starts with an underscore we retry
    // without the underscore. I (DWD) don't know why.
    if (name[0] == 1 && str[0] == '_') {
        ++str;
        addr = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(str);
    }

    if (!addr) {
        addr = dlsym(RTLD_DEFAULT, str);
        if (addr) {
            llvm::sys::DynamicLibrary::AddSymbol(str, addr);
        }
    }

    return addr;
}

extern "C" {
    typedef double (*external_fcn_dd_t)(double);
    typedef uint32_t (*external_fcn_u32d_t)(double);
    typedef double (*external_fcn_ddu32_t)(double, uint32_t);
    typedef double (*external_fcn_ddd_t)(double, double);
    typedef uint64_t (*external_fcn_t)(...);
}

uint64_t ExternalDispatcher::double_to_rawbits(double value) {
    uint64_t bits = 0;
    memcpy(&bits, &value, 8);
    return bits;
}

double ExternalDispatcher::rawbits_to_double(uint64_t bits) {
    double value = 0.0;
    memcpy(&value, &bits, 8);
    return value;
}

bool ExternalDispatcher::call(const std::string& targetName, void *targetAddr, const Arguments &args, uint64_t *result,
                             std::stringstream &err) {
    if (targetName == "exp2" || targetName == "log" || targetName == "tan" ||
        targetName == "rint" || targetName == "fabs" || targetName == "floor" ||
        targetName == "ceil" || targetName == "sin" || targetName == "cos") { // double func(double)
        auto targetFunc = (external_fcn_dd_t) targetAddr;
        *result = double_to_rawbits(targetFunc(rawbits_to_double(args[0])));
    } else if (targetName == "isinf" || targetName == "isnan") { // uint32_t func(double)
        auto targetFunc = (external_fcn_u32d_t) targetAddr;
        *result = targetFunc(rawbits_to_double(args[0]));
    } else if (targetName == "ldexp") { // double func(double, uint32_t)
        auto targetFunc = (external_fcn_ddu32_t) targetAddr;
        *result = double_to_rawbits(targetFunc(rawbits_to_double(args[0]), args[1]));
    } else if (targetName == "atan2") { // double func(double, double)
        auto targetFunc = (external_fcn_ddd_t) targetAddr;
        *result = double_to_rawbits(targetFunc(rawbits_to_double(args[0]), rawbits_to_double(args[1])));
    } else {
        auto targetFunc = (external_fcn_t) targetAddr;
        switch (args.size()) {
            case 0:
                *result = targetFunc();
                break;
            case 1:
                *result = targetFunc(args[0]);
                break;
            case 2:
                *result = targetFunc(args[0], args[1]);
                break;
            case 3:
                *result = targetFunc(args[0], args[1], args[2]);
                break;
            case 4:
                *result = targetFunc(args[0], args[1], args[2], args[3]);
                break;
            case 5:
                *result = targetFunc(args[0], args[1], args[2], args[3], args[4]);
                break;
            case 6:
                *result = targetFunc(args[0], args[1], args[2], args[3], args[4], args[5]);
                break;
            case 7:
                *result = targetFunc(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
                break;
            case 8:
                *result = targetFunc(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
                break;
            default: {
                err << "External function has too many parameters";
                return false;
            }
        }
    }
    return true;
    }
}
