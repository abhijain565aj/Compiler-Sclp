#include "program.hh"
#include "assert.h"
#include <algorithm>

Sclp::Program::Program(
    std::unique_ptr<SymbolTable> globals, 
    std::unique_ptr<FunctionSymbolTable> _funcDeclarations,
    std::vector<std::shared_ptr<Procedure>> _procedures
): globalSymTable(std::move(globals)), funcDeclarations(std::move(_funcDeclarations)),procedures(std::move(_procedures)){
}

std::vector<std::shared_ptr<Sclp::Procedure>> Sclp::Program::getSortedProcedures(){
    auto copyProcedures = procedures;
    sort(copyProcedures.begin(),copyProcedures.end(),
    [&](std::shared_ptr<Procedure>& lhs, std::shared_ptr<Procedure>& rhs){
        return lhs->name < rhs->name;
    });
    return copyProcedures;
}

std::vector<std::shared_ptr<Sclp::FunctionSymbolEntry>> Sclp::Program::getSortedFuncDeclarations(){
    auto copyTable = funcDeclarations->table;
    sort(copyTable.begin(),copyTable.end(),
        [&](std::shared_ptr<FunctionSymbolEntry>& lhs, std::shared_ptr<FunctionSymbolEntry>& rhs){
        return lhs->getName() < rhs->getName();
    });
    return copyTable;
}

size_t Sclp::Program::insertString(std::string str){
    for (size_t i = 0; i < stringTable.size(); i++){
        if (stringTable[i] == str) return i;
    }
    stringTable.push_back(str);
    return stringTable.size()-1;
}

Sclp::SymbolEntry::SymbolEntry(std::string _name, dataType dT, symbolType sT): name(_name), dType(dT), sType(sT) {}

Sclp::SymbolTable::SymbolTable() {}

Sclp::SymbolTable::SymbolTable(
    dataType t, 
    std::vector<std::string> symbols,
    SymbolEntry::symbolType sType
){
    for (auto &s: symbols){
        insert(std::make_shared<SymbolEntry>(std::move(s), t, sType));
    }
}

Sclp::FunctionSymbolEntry::FunctionSymbolEntry(std::string _name, dataType _return_type, std::unique_ptr<SymbolTable> _paramList): 
procedure(std::make_shared<Procedure>(_return_type,_name,std::move(_paramList),std::make_unique<SymbolTable>(),std::vector<std::unique_ptr<Sclp::StatementAst>>())),definitionExists(false) {}

Sclp::FunctionSymbolEntry::FunctionSymbolEntry(std::shared_ptr<Procedure> _procedure):procedure(std::move(_procedure)),definitionExists(true){}

std::string Sclp::FunctionSymbolEntry::getName() {return procedure->name;}
Sclp::dataType Sclp::FunctionSymbolEntry::getReturnType() {return procedure->return_type;}
std::unique_ptr<Sclp::SymbolTable>& Sclp::FunctionSymbolEntry::getParamList() {return procedure->paramList;}

Sclp::FunctionSymbolTable::FunctionSymbolTable() {}

Sclp::Procedure::Procedure(
    dataType _return_type, 
    std::string _name, 
    std::unique_ptr<SymbolTable> _paramList, 
    std::unique_ptr<SymbolTable> _localSymbols, 
    std::vector<std::unique_ptr<Sclp::StatementAst>> _body
) : name(_name), 
return_type(_return_type), 
localSymbols(std::move(_localSymbols)), 
paramList(std::move(_paramList)), 
body(std::move(_body)) {

}

void Sclp::SymbolTable::merge(std::unique_ptr<SymbolTable> other){
    for (auto &e: other->table){
        insert(std::move(e));
    }
}

void Sclp::SymbolTable::insert(std::shared_ptr<SymbolEntry> entry){
    table.push_back(entry);
}

void Sclp::SymbolTable::insertAtStart(std::shared_ptr<SymbolEntry> entry){
    table.insert(table.begin(), entry);
}

int Sclp::SymbolTable::getMaxOffset(){
    int max_offset = 0;
    for(auto it = table.begin();it!=table.end();it++){
        max_offset = std::max(it->get()->end_offset,max_offset);
    }
    return max_offset;
}

void Sclp::FunctionSymbolTable::insert(std::shared_ptr<FunctionSymbolEntry> entry){
    table.push_back(entry);
}


std::shared_ptr<Sclp::SymbolEntry> Sclp::SymbolTable::lookup(const std::string &name){
    for (auto it=table.begin(); it!=table.end(); it++){
        if (it->get()->name == name) return *it;
    }
    return nullptr;
}

std::shared_ptr<Sclp::FunctionSymbolEntry> Sclp::FunctionSymbolTable::lookup(const std::string &name){
    for (auto it=table.begin(); it!=table.end(); it++){
        if (it->get()->getName() == name) return *it;
    }
    return nullptr;
}

void Sclp::Procedure::accept(AstVisitor &v){
    v.visit(*this);
}

