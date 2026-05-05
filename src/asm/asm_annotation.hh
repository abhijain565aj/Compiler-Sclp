#ifndef ASM_ANNOTATION_HH
#define ASM_ANNOTATION_HH

#include <memory>
#include "asm.hh"
#include <optional>

namespace Sclp{

class AsmStmtAnnotation{
public:
    size_t start;
    size_t end;

    AsmStmtAnnotation() = default;
    AsmStmtAnnotation(size_t _start, size_t _end): start(_start), end(_end){}
};

class AsmOpdAnnotation{
public:
    using VALUE = std::shared_ptr<AsmOpd>;

    VALUE value;

    AsmOpdAnnotation() = default;
    AsmOpdAnnotation(VALUE _value): value(_value){}
};

}

#endif