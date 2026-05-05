#ifndef TAC_VISITOR
#define TAC_VISITOR
#include "declarations.hh"

namespace Sclp{
class TacVisitor{
public:
    virtual ~TacVisitor() = default;
    
    virtual void visit(TacOpd& opd) = 0;
    virtual void visit(ConstTacOpd& opd) = 0;
    virtual void visit(DoubleConstTacOpd& opd) = 0;
    virtual void visit(IntConstTacOpd& opd) = 0;
    virtual void visit(StrConstTacOpd& opd) = 0;
    virtual void visit(LabelTacOpd& opd) = 0;
    virtual void visit(VarTempTacOpd& opd) = 0;
    virtual void visit(TempraryTacOpd& opd) = 0;
    virtual void visit(VariableTacOpd& opd) = 0;

    virtual void visit(TacStmt& stmt) = 0;
    virtual void visit(AsgnTacStmt& stmt) = 0;
    virtual void visit(ComputeTacStmt& stmt) = 0;
    virtual void visit(IOTacStmt& stmt) = 0;
    virtual void visit(GotoTacStmt& stmt) = 0;
    virtual void visit(LabelTacStmt& stmt) = 0;
    virtual void visit(IfGotoTacStmt& stmt) = 0;
    virtual void visit(CallTacStmt& stmt) = 0;
    virtual void visit(ReturnTacStmt& stmt) = 0;
};
}
#endif