#ifndef ASM_BUILDER
#define ASM_BUILDER
#include "declarations.hh"
#include "tac_visitor.hh"
#include "ast_visitor.hh"
#include "rtl.hh"
#include <cstddef>
#include <unordered_map>
#include <memory>

namespace Sclp{
class AsmBuilder : public RtlVisitor{
    std::shared_ptr<Program> current_program;
    std::shared_ptr<Procedure> current_procedure;
    
public:
    AsmBuilder(std::shared_ptr<Program> prog);
    void visit(std::shared_ptr<Procedure> proc);

    void visit(RtlOpd& opd) override;
    void visit(ConstRtlOpd& opd) override;
    void visit(DoubleConstRtlOpd& opd) override;
    void visit(IntConstRtlOpd& opd) override;
    void visit(StrConstRtlOpd& opd) override;
    void visit(LabelRtlOpd& opd) override;
    void visit(VariableRtlOpd& opd) override;
    void visit(RegisterRtlOpd& opd) override;

    void visit(RtlStmt& stmt) override;
    void visit(ComputeRtlStmt& stmt) override;
    void visit(WriteRtlStmt& stmt) override;
    void visit(ReadRtlStmt& stmt) override;
    void visit(MoveRtlStmt& stmt) override;
    void visit(StackOpRtlStmt& stmt) override;
    void visit(LabelRtlStmt& stmt) override;
    void visit(ControlFlowRtlStmt& stmt) override;
    void visit(GotoRtlStmt& stmt) override;
    void visit(IfGotoRtlStmt& stmt) override;
    void visit(ReturnRtlStmt& stmt) override;
    void visit(CallRtlStmt& stmt) override;
    
private:
    inline static const std::unordered_map<Sclp::dataType, int> sizeofDType = {
        {Sclp::dataType::Int, 4},
        {Sclp::dataType::Float, 8},
        {Sclp::dataType::String, 4},
        {Sclp::dataType::Bool, 4}
    };

    size_t emit(std::shared_ptr<AsmStmt> tac);
    void fillGlobalOffsets();
    void fillOffsetsInSymTab(); 
    void getPrologue();
    void getEpilogue();
    size_t getCodeLineNo();
    // void handleFloatRbOp(ComputeTacStmt& stmt);
};
}
#endif