#ifndef TAC_BUILDER_HH
#define TAC_BUILDER_HH
#include <map>
#include "ast_visitor.hh"
#include "program.hh"
#include "tac.hh"
#include "declarations.hh"

namespace Sclp{

class TacBuilder : public AstVisitor{
    std::shared_ptr<TempraryTacOpd> return_s0;
    std::shared_ptr<Procedure> proc;

    std::map<std::shared_ptr<Procedure>,std::shared_ptr<LabelTacOpd>> returnLabel;
    std::shared_ptr<LabelTacOpd> getReturnLabel();

public:

    TacBuilder(std::shared_ptr<Program> p);
    void visit(Procedure&) override;

    void visit(Ast&) override;
    void visit(StatementAst&) override;
    void visit(ExpressionAst&) override;
    void visit(BaseExprAst&) override;
    void visit(UnaryExprAst&) override;
    void visit(BinaryExprAst&) override;
    void visit(TernaryExprAst&) override;
    void visit(FunctionCallAst&) override;
    void visit(NameExprAst&) override;
    void visit(NumberExprAst<int>&) override;
    void visit(NumberExprAst<double>&) override;
    void visit(StringExprAst&) override;
    void visit(ArithmeticUnaryExprAst&) override;
    void visit(BooleanUnaryExprAst&) override;
    void visit(ArithmeticBinaryExprAst&) override;
    void visit(BooleanBinaryExprAst&) override;
    void visit(RelationalBinaryExprAst&) override;
    void visit(ConditionalExprAst&) override;
    void visit(AssignmentStmtAst&) override;
    void visit(ReadStmtAst&) override;
    void visit(WriteStmtAst&) override;
    void visit(IterationStmtAst&) override;
    void visit(SelectionStmtAst&) override;
    void visit(SequenceStmtAst&) override;
    void visit(ReturnStmtAst&) override;
    void visit(CallStmtAst&) override;
    
private:
    std::shared_ptr<TempraryTacOpd> getNewTemp(dataType t);
    std::shared_ptr<TempraryTacOpd> getNewSTemp(dataType t, bool goesAtStart=false);
    std::shared_ptr<LabelTacOpd> getNewLabel();
    
    size_t getCodeLineNo();
    size_t emit(std::shared_ptr<TacStmt> tac);
    void put(std::shared_ptr<TacStmt> tac, size_t pos);
};

}

#endif