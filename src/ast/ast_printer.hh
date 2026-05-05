#ifndef AST_PRINTER_HH
#define AST_PRINTER_HH
#include <string>
#include <map>
#include "ast_visitor.hh"
#include "program.hh"

namespace Sclp{

static const std::map<dataType,std::string> TYPE_NAME = {
    {dataType::Int,"<int>"},
    {dataType::Void,"<void>"},
    {dataType::Float,"<float>"},
    {dataType::String,"<string>"},
    {dataType::Bool,"<bool>"}
};

class AstPrinter : public AstVisitor{    
    std::ofstream ast_file_stream;
    int indent_level;
    template<typename F, typename... Args>
    void indent(int x, F&& f, Args&&... args);
    template<typename F, typename... Args>
    void indent_stmt(int x, F&& f, Args&&... args);
public:
    AstPrinter(std::shared_ptr<Program> program, const std::string& filename);
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
};

}

#endif