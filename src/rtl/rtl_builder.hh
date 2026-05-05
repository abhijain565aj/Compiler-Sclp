#ifndef RTL_BUILDER
#define RTL_BUILDER
#include "declarations.hh"
#include "tac_visitor.hh"
#include "ast_visitor.hh"
#include "rtl.hh"
#include <cstddef>
#include <memory>

namespace Sclp{
class RtlBuilder : public TacVisitor{
    std::shared_ptr<RegisterAllocator> reg_allocator;
    std::shared_ptr<Program> program;
    std::shared_ptr<Procedure> current_procedure;
    
public:
    RtlBuilder(std::shared_ptr<Program> prog);
    void visit(std::shared_ptr<Procedure> proc);

    void visit(TacOpd& opd) override;
    void visit(ConstTacOpd& opd) override;
    void visit(DoubleConstTacOpd& opd) override;
    void visit(IntConstTacOpd& opd) override;
    void visit(StrConstTacOpd& opd) override;
    void visit(LabelTacOpd& opd) override;
    void visit(VarTempTacOpd& opd) override;
    void visit(TempraryTacOpd& opd) override;
    void visit(VariableTacOpd& opd) override;

    void visit(TacStmt& stmt) override;
    void visit(AsgnTacStmt& stmt) override;
    void visit(ComputeTacStmt& stmt) override;
    void visit(IOTacStmt& stmt) override;
    void visit(GotoTacStmt& stmt) override;
    void visit(LabelTacStmt& stmt) override;
    void visit(IfGotoTacStmt& stmt) override;
    void visit(CallTacStmt& stmt) override;
    void visit(ReturnTacStmt& stmt) override;

private:
    friend class RegisterAllocator;
    size_t emit(std::shared_ptr<RtlStmt> tac);
    size_t getCodeLineNo();
    void handleFloatRbOp(ComputeTacStmt& stmt);
};
}
#endif