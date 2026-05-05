#ifndef PROGRAM_HH
#define PROGRAM_HH

#include <vector>
#include <memory>
#include <string>

#include "ast.hh"
#include "tac.hh"
#include "rtl.hh"
#include "asm.hh"
#include "asm_annotation.hh"
#include "declarations.hh"

namespace Sclp
{

class Program {
public:
    std::unique_ptr<SymbolTable> globalSymTable;
    std::vector<std::string> stringTable;

    std::unique_ptr<FunctionSymbolTable> funcDeclarations;
    std::vector<std::shared_ptr<Procedure>> procedures;

    Program(std::unique_ptr<SymbolTable> globals, std::unique_ptr<FunctionSymbolTable> funcDeclarations, std::vector<std::shared_ptr<Procedure>> procedures);

    std::vector<std::shared_ptr<Procedure>> getSortedProcedures();
    std::vector<std::shared_ptr<FunctionSymbolEntry>> getSortedFuncDeclarations();

    size_t insertString(std::string str);
};

class SymbolEntry{
public:

    enum class symbolType{
        PARAM,
        VAR,
        STEMP,
        RETURN_STEMP
    };

    std::string name;
    dataType dType;
    symbolType sType;

    int start_offset;
    int end_offset;

    SymbolEntry(std::string name, dataType dT, symbolType sT);
};

class SymbolTable {
public:    
    std::vector<std::shared_ptr<SymbolEntry>> table;

    SymbolTable();
    SymbolTable(dataType t, std::vector<std::string> symbols, SymbolEntry::symbolType sType);

    void merge(std::unique_ptr<SymbolTable> other);
    void insert(std::shared_ptr<SymbolEntry> entry);
    void insertAtStart(std::shared_ptr<SymbolEntry> entry);
    int getMaxOffset();
    std::shared_ptr<SymbolEntry> lookup(const std::string &name);
};

class FunctionSymbolEntry{
public:
    std::shared_ptr<Procedure> procedure; //pointer to definition
    bool definitionExists;

    FunctionSymbolEntry(std::string name, dataType return_type, std::unique_ptr<SymbolTable> paramList);
    FunctionSymbolEntry(std::shared_ptr<Procedure> procedure);

    std::string getName();
    dataType getReturnType();
    std::unique_ptr<SymbolTable>& getParamList();
};

class FunctionSymbolTable {
public:    
    std::vector<std::shared_ptr<FunctionSymbolEntry>> table;

    FunctionSymbolTable();

    void insert(std::shared_ptr<FunctionSymbolEntry> entry);
    std::shared_ptr<FunctionSymbolEntry> lookup(const std::string &name);
};

class Procedure {
public: 
    std::string name;
    dataType return_type;

    std::unique_ptr<SymbolTable> localSymbols;

    std::unique_ptr<SymbolTable> paramList;

    std::vector<std::unique_ptr<Sclp::StatementAst>> body;
    std::vector<std::shared_ptr<Sclp::TacStmt>> tac_code;
    std::vector<std::shared_ptr<Sclp::RtlStmt>> rtl_code;
    std::vector<std::shared_ptr<Sclp::AsmStmt>> asm_code;

    std::shared_ptr<Sclp::AsmStmtAnnotation> prologue;
    std::shared_ptr<Sclp::AsmStmtAnnotation> epilogue;

    Procedure(
        dataType return_type, 
        std::string name, 
        std::unique_ptr<SymbolTable> paramList, 
        std::unique_ptr<SymbolTable> localSymbols, 
        std::vector<std::unique_ptr<Sclp::StatementAst>> body);

    void accept(AstVisitor &v);
};

} // namespace Sclp

#endif