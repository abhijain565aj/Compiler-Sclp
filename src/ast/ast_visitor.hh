#ifndef AST_VISITOR_HH
#define AST_VISITOR_HH

#include "declarations.hh"
namespace Sclp{

class AstVisitor{
public:
    AstVisitor() = default;
    virtual ~AstVisitor() = default;

    virtual void visit(Procedure&) = 0;

    virtual void visit(Ast&) = 0;
    virtual void visit(StatementAst&) = 0;
    virtual void visit(ExpressionAst&) = 0;
    virtual void visit(BaseExprAst&) = 0;
    virtual void visit(UnaryExprAst&) = 0;
    virtual void visit(BinaryExprAst&) = 0;
    virtual void visit(TernaryExprAst&) = 0;

    virtual void visit(FunctionCallAst&) = 0;
    virtual void visit(NameExprAst&) = 0;
    virtual void visit(NumberExprAst<int>&) = 0;
    virtual void visit(NumberExprAst<double>&) = 0;
    virtual void visit(StringExprAst&) = 0;
    virtual void visit(ArithmeticUnaryExprAst&) = 0;
    virtual void visit(BooleanUnaryExprAst&) = 0;
    virtual void visit(ArithmeticBinaryExprAst&) = 0;
    virtual void visit(BooleanBinaryExprAst&) = 0;
    virtual void visit(RelationalBinaryExprAst&) = 0;
    virtual void visit(ConditionalExprAst&) = 0;

    virtual void visit(AssignmentStmtAst&) = 0;
    virtual void visit(ReadStmtAst&) = 0;
    virtual void visit(WriteStmtAst&) = 0;
    virtual void visit(IterationStmtAst&) = 0;
    virtual void visit(SelectionStmtAst&) = 0;
    virtual void visit(SequenceStmtAst&) = 0;
    virtual void visit(ReturnStmtAst&) = 0;
    virtual void visit(CallStmtAst&) = 0;
};

}

#endif