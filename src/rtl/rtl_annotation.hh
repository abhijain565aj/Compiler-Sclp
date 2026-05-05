#ifndef RTL_ANNOTATION_HH
#define RTL_ANNOTATION_HH

#include <memory>
#include "rtl.hh"
#include <optional>

namespace Sclp{

class RtlStmtAnnotation{
public:
    size_t start;
    size_t end;

    RtlStmtAnnotation() = default;
    RtlStmtAnnotation(size_t _start, size_t _end): start(_start), end(_end){}
};

class RtlOpdAnnotation{
public:
    using VALUE = std::shared_ptr<RtlOpd>;

    VALUE value;

    RtlOpdAnnotation() = default;
    RtlOpdAnnotation(VALUE _value): value(_value){}
};

}

#endif