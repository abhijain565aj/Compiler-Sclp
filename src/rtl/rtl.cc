#include "rtl.hh"

Sclp::MoveRtlStmt::MoveRtlStmt(std::shared_ptr<RtlOpd> _lhs, std::shared_ptr<RtlOpd> _rhs1, std::shared_ptr<RtlOpd> _rhs2 , std::optional<MoveType> _type): lhs(_lhs), rhs1(_rhs1),rhs2(_rhs2) {
    if(rhs2){
        type = _type.value();
    }
    else if(std::dynamic_pointer_cast<IntConstRtlOpd>(rhs1)){
        type = MoveType::iLoad;
    }
    else if(std::dynamic_pointer_cast<DoubleConstRtlOpd>(rhs1)){
        type = MoveType::iLoadD;
    }
    else if(std::dynamic_pointer_cast<StrConstRtlOpd>(rhs1)){
        type = MoveType::load_addr;
    }
    else if(std::dynamic_pointer_cast<RegisterRtlOpd>(lhs) && std::dynamic_pointer_cast<RegisterRtlOpd>(rhs1)){
        auto lhsReg = std::dynamic_pointer_cast<RegisterRtlOpd>(lhs);
        type = (lhsReg->reg->t == dataType::Int)?MoveType::move:MoveType::moveD;
    }
    else if(std::dynamic_pointer_cast<RegisterRtlOpd>(lhs)){
        auto lhsReg = std::dynamic_pointer_cast<RegisterRtlOpd>(lhs);
        type = (lhsReg->reg->t == dataType::Int)?MoveType::load:MoveType::loadD;
    }
    else if(std::dynamic_pointer_cast<RegisterRtlOpd>(rhs1)){
        auto rhsReg = std::dynamic_pointer_cast<RegisterRtlOpd>(rhs1);
        type = (rhsReg->reg->t == dataType::Int)?MoveType::store:MoveType::storeD;
    }
}