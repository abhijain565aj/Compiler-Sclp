#ifndef RTL_VISITOR
#define RTL_VISITOR
#include "declarations.hh"

namespace Sclp{
class RtlVisitor{
public:
    virtual ~RtlVisitor() = default;
    
    virtual void visit(RtlOpd& opd) = 0;
    virtual void visit(ConstRtlOpd& opd) = 0;
    virtual void visit(DoubleConstRtlOpd& opd) = 0;
    virtual void visit(IntConstRtlOpd& opd) = 0;
    virtual void visit(StrConstRtlOpd& opd) = 0;
    virtual void visit(LabelRtlOpd& opd) = 0;
    virtual void visit(VariableRtlOpd& opd) = 0;
    virtual void visit(RegisterRtlOpd& opd) = 0;

    virtual void visit(RtlStmt& stmt) = 0;
    virtual void visit(ComputeRtlStmt& stmt) = 0;
    virtual void visit(WriteRtlStmt& stmt) = 0;
    virtual void visit(ReadRtlStmt& stmt) = 0;
    virtual void visit(MoveRtlStmt& stmt) = 0;
    virtual void visit(StackOpRtlStmt& stmt) = 0;
    virtual void visit(LabelRtlStmt& stmt) = 0;
    virtual void visit(ControlFlowRtlStmt& stmt) = 0;
    virtual void visit(GotoRtlStmt& stmt) = 0;
    virtual void visit(IfGotoRtlStmt& stmt) = 0;
    virtual void visit(ReturnRtlStmt& stmt) = 0;
    virtual void visit(CallRtlStmt& stmt) = 0;
};
}
#endif