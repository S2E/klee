//===-- ExternalDispatcher.h ------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXTERNALDISPATCHER_H
#define KLEE_EXTERNALDISPATCHER_H

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include <llvm/ADT/SmallVector.h>

namespace klee {
class ExternalDispatcher {
private:
public:
    typedef llvm::SmallVector<uint64_t, 8> Arguments;

    ExternalDispatcher();
    virtual ~ExternalDispatcher();

    virtual void *resolveSymbol(const std::string &name);
    virtual bool call(const std::string& targetName, void* targetAddr, const Arguments &args, uint64_t *result, std::stringstream &err);

    uint64_t double_to_rawbits(double value);

    double rawbits_to_double(uint64_t bits);
};
}

#endif
