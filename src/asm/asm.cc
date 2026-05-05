#include <memory>
#include "rtl.hh"
#include "asm.hh"

Sclp::MoveAsmStmt::MoveAsmStmt(MoveType mt, std::shared_ptr<AsmOpd> _lhs, std::shared_ptr<AsmOpd> _rhs1, std::shared_ptr<AsmOpd> _rhs2) : type(mt), lhs(_lhs), rhs1(_rhs1), rhs2(_rhs2){}
