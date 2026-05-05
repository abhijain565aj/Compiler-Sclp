#ifndef TAC_ANNOTATION_HH
#define TAC_ANNOTATION_HH
#include <optional>
#include <memory>
#include "tac.hh"

namespace Sclp{

class TacAnnotation{
public:

    size_t start;
    size_t end;

    using PLACE = std::optional<std::shared_ptr<VarTempTacOpd>>;
    using VALUE = std::optional<std::shared_ptr<ConstTacOpd>>;

    PLACE place;
    VALUE value;
    
    TacAnnotation() = default;

    TacAnnotation(size_t _start, size_t _end, PLACE _place, VALUE _value): start(_start), end(_end), place(_place), value(_value){}


    std::shared_ptr<Sclp::TacOpd> getPV(){
        if (place.has_value()) return place.value();
        if (value.has_value()) return value.value();
    
        throw std::runtime_error("Cant find Value or Place");
    }
};

}

#endif