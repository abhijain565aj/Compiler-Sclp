#ifndef AST_TYPE_CHECK_HH
#define AST_TYPE_CHECK_HH
#include <string>
#include <fstream>
#include "ast_visitor.hh"
#include "program.hh"

namespace Sclp{

class AstTypeCheck : public AstVisitor{
private:
    void checkRedeclarations(const SymbolTable& table);
    void checkRedeclarations(const FunctionSymbolTable& table);
    void checkRedeclarations(const SymbolTable& paramList, const SymbolTable& localSymbols);
    void checkConflicts(const SymbolTable& paramList, const FunctionSymbolTable& funcTable);
    
    void checkMainProcedure();
    void checkIfVarVoid(const SymbolTable& symtab);
    std::shared_ptr<FunctionSymbolEntry> lookupFunction(std::string& name);
    dataType getType(std::string &name);

    std::shared_ptr<Program> current_program;
    std::shared_ptr<Procedure> current_procedure;
    bool returnExists;
    
    void error(location &loc, std::string msg="");
    void error(std::string msg="");
public:
    AstTypeCheck(std::shared_ptr<Program>);
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