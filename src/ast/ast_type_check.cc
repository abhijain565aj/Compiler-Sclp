#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_set>
#include "ast_type_check.hh"

void Sclp::AstTypeCheck::error(location &loc, std::string msg){
    std::cerr<<"Semantic Error:"<<msg<<std::endl;
    std::cerr<<"Position "<<loc.begin<<" "<<loc.end<<std::endl;
    exit(1);
}

void Sclp::AstTypeCheck::error(std::string msg){
    std::cerr<<"Semantic Error:"<<msg<<std::endl;
    exit(1);
}

std::shared_ptr<Sclp::FunctionSymbolEntry> Sclp::AstTypeCheck::lookupFunction(std::string& name){
    return current_program->funcDeclarations->lookup(name);
}

Sclp::dataType Sclp::AstTypeCheck::getType(std::string& varName){
    for (auto s: current_procedure->paramList->table){
        if(s->name == varName) 
            return s->dType;
    }
    
    auto symbolTable = current_procedure->localSymbols->table;
    auto sym = current_procedure->localSymbols->lookup(varName);
    if (sym) return sym->dType;    

    if (!current_program) 
        throw std::runtime_error("Program not defined");

    sym = current_program->globalSymTable->lookup(varName);
    if (sym) return sym->dType;

    std::cerr<<"Error: Variable not in Scope: "<<varName<<std::endl;
    exit(1);
}

void Sclp::AstTypeCheck::checkRedeclarations(const SymbolTable& table){
    std::unordered_set<std::string> used_vars;

    for (auto sym: table.table){
        if (used_vars.find(sym->name)!=used_vars.end()) 
            return error("Redeclaration of variable: "+sym->name);
        used_vars.insert(sym->name);
    }
}

void Sclp::AstTypeCheck::checkRedeclarations(const FunctionSymbolTable& table){
    std::unordered_set<std::string> used_vars;

    for (auto sym: table.table){
        if (used_vars.find(sym->getName())!=used_vars.end()) 
            return error("Redeclaration of variable: "+sym->getName());
        used_vars.insert(sym->getName());
    }
}

void Sclp::AstTypeCheck::checkRedeclarations(const SymbolTable& paramList, const SymbolTable& localSymbols){
    std::unordered_set<std::string> used_vars;

    for (auto sym: paramList.table){
        if (used_vars.find(sym->name)!=used_vars.end()) 
            return error("Redeclaration of variable: "+sym->name);
        used_vars.insert(sym->name);
    }

    for (auto sym: localSymbols.table){
        if (used_vars.find(sym->name)!=used_vars.end()) 
            return error("Redeclaration of variable: "+sym->name);
        used_vars.insert(sym->name);
    }
}

void Sclp::AstTypeCheck::checkConflicts(const SymbolTable& paramList, const FunctionSymbolTable& funcTable){
    std::unordered_set<std::string> used_vars;

    used_vars.insert("main_");

    for (auto sym: funcTable.table){
        if (used_vars.find(sym->getName())!=used_vars.end()) 
            return error("Redeclaration of variable: "+sym->getName());
        used_vars.insert(sym->getName());
    }

    for (auto sym: paramList.table){
        if (used_vars.find(sym->name)!=used_vars.end()) 
            return error("Redeclaration of variable: "+sym->name);
        used_vars.insert(sym->name);
    }
}

void Sclp::AstTypeCheck::checkMainProcedure(){
    for(auto p:current_program->procedures){
        if(p->name == "main") return;
    }
    error("No function definition for main");
}

void Sclp::AstTypeCheck::checkIfVarVoid(const SymbolTable& symtab){
    for (auto sym: symtab.table){
        if (sym->dType == Sclp::dataType::Void){
            return error("Variable declared void");
        }
    }
}

Sclp::AstTypeCheck::AstTypeCheck(std::shared_ptr<Program> p){
    current_program = p;
    for(auto &func: p->procedures){
        auto entry = lookupFunction(func->name);
        if(entry) {
            auto& paramList1 = entry->getParamList()->table;
            auto& paramList2 = func->paramList->table;
            if(paramList1.size()!=paramList2.size()){
                error("Size of parameter lists in declaration and definition do not match");
            }else{
                for(size_t i=0; i<paramList1.size();i++){
                    if(paramList1[i]->dType!=paramList2[i]->dType){
                        error("Types of parameters in declaration and definition do not match");
                    }
                }
            }
            entry->procedure = func;
            entry->definitionExists = true;
        }
    }

    checkMainProcedure();
    checkIfVarVoid(*p->globalSymTable);
    checkRedeclarations(*p->globalSymTable);
    checkRedeclarations(*p->funcDeclarations);

    checkConflicts(*p->globalSymTable, *p->funcDeclarations);

    for (auto &func: p->procedures){
        auto entry = lookupFunction(func->name);
        if (!entry){
            p->funcDeclarations->insert(std::move(std::make_shared<FunctionSymbolEntry>(func)));
        }
        current_procedure = func;
        func->accept(*this);
    }
}

void Sclp::AstTypeCheck::visit(Procedure& p){
    returnExists = false;

    checkIfVarVoid(*p.localSymbols);
    checkRedeclarations(*p.paramList, *p.localSymbols);
    checkConflicts(*p.paramList, *current_program->funcDeclarations);
    checkConflicts(*p.localSymbols, *current_program->funcDeclarations);

    auto decl = lookupFunction(p.name);
    if (decl){
        if (p.return_type!=decl->getReturnType()) 
            return error("Return Type does not match in function signature: "+ p.name);
        
        if (p.paramList->table.size() != decl->getParamList()->table.size()) 
            return error("Parameters do not match in function signature: "+ p.name);

        for (size_t i=0; i<p.paramList->table.size(); i++){
            if (p.paramList->table[i]->dType != decl->getParamList()->table[i]->dType) 
                return error("Parameters do not match in function signature: "+ p.name);
        }
    }

    for (auto &stmt: p.body){
        stmt->accept(*this);
    }

    if(!returnExists && p.return_type != dataType::Void){
        error("Missing return statement in a non-void procedure");
    }
}

void Sclp::AstTypeCheck::visit(Ast&){
}

void Sclp::AstTypeCheck::visit(StatementAst&){
}

void Sclp::AstTypeCheck::visit(ExpressionAst&){
}

void Sclp::AstTypeCheck::visit(BaseExprAst&){
}

void Sclp::AstTypeCheck::visit(UnaryExprAst&){
}

void Sclp::AstTypeCheck::visit(BinaryExprAst&){
}

void Sclp::AstTypeCheck::visit(TernaryExprAst&){
}

void Sclp::AstTypeCheck::visit(FunctionCallAst& exp){
    for(auto& x:exp.args){
        x->accept(*this);
    }
    std::shared_ptr<FunctionSymbolEntry> entry = lookupFunction(exp.name);
    if(!entry){
        error(exp.loc,"Procedure corresponding to the name is not found");
    }
    if(exp.args.size()!=entry->getParamList()->table.size()){
        error(exp.loc,"unequal number of funtion arguments provided");
    }
    if(!entry->definitionExists){
        error("Called procedure is not defined");
    }
    auto it1 = exp.args.begin();
    auto it2 = entry->getParamList()->table.begin();
    for(;it1!=exp.args.end();++it1,++it2){
        if((*it1)->getType()!=(*it2)->dType){
            error(exp.loc,"argument types don't match parameter types");
        }
    }
    exp.setType(entry->getReturnType());
}

void Sclp::AstTypeCheck::visit(NameExprAst& exp){
    exp.setType(getType(exp.name));
}

void Sclp::AstTypeCheck::visit(NumberExprAst<int>& exp){
    exp.setType(dataType::Int);
}

void Sclp::AstTypeCheck::visit(NumberExprAst<double>& exp){
    exp.setType(dataType::Float);
}

void Sclp::AstTypeCheck::visit(StringExprAst& exp){
    exp.setType(dataType::String);
}

void Sclp::AstTypeCheck::visit(ArithmeticUnaryExprAst& exp){
    exp.operand->accept(*this);
    
    auto opType = exp.operand->getType();
    if(opType != dataType::Int && opType != dataType::Float)
        return error(exp.loc,typeid(exp).name());
    exp.setType(opType);
}

void Sclp::AstTypeCheck::visit(BooleanUnaryExprAst& exp){
    exp.operand->accept(*this);
    if(exp.operand->getType() != dataType::Bool)
        return error(exp.loc,typeid(exp).name());
    exp.setType(exp.operand->getType());
}

void Sclp::AstTypeCheck::visit(ArithmeticBinaryExprAst& exp){
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);

    auto lhsType = exp.lhs->getType();
    auto rhsType = exp.rhs->getType();
    if(lhsType != rhsType)
        return error(exp.loc,typeid(exp).name());
    if(lhsType!=dataType::Int && lhsType!=dataType::Float)
        return error(exp.loc,typeid(exp).name());
    exp.setType(lhsType);
}

void Sclp::AstTypeCheck::visit(BooleanBinaryExprAst& exp){
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);

    auto lhsType = exp.lhs->getType();
    auto rhsType = exp.rhs->getType();
    if(lhsType != rhsType)
        return error(exp.loc,typeid(exp).name());
    if(lhsType!=dataType::Bool)
        return error(exp.loc,typeid(exp).name());
    exp.setType(dataType::Bool);
}

void Sclp::AstTypeCheck::visit(RelationalBinaryExprAst& exp){
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);

    auto lhsType = exp.lhs->getType();
    auto rhsType = exp.rhs->getType();
    if(lhsType != rhsType)
        return error(exp.loc,typeid(exp).name());
    if(lhsType!=dataType::Int && lhsType!=dataType::Float)
        return error(exp.loc,typeid(exp).name());
    exp.setType(dataType::Bool);
}

void Sclp::AstTypeCheck::visit(ConditionalExprAst& exp){
    exp.expr1->accept(*this);
    exp.expr2->accept(*this);
    exp.expr3->accept(*this);
    
    auto condType = exp.expr1->getType();
    auto ifExprType = exp.expr2->getType();
    auto elseExprType = exp.expr3->getType();

    if(condType!=dataType::Bool) 
        return error(exp.loc,typeid(exp).name());
    if(ifExprType!=elseExprType)
        return error(exp.loc,typeid(exp).name());
    if(ifExprType==dataType::Void)
        return error(exp.loc,typeid(exp).name());

    exp.setType(ifExprType);
}

void Sclp::AstTypeCheck::visit(AssignmentStmtAst& exp){
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);

    if(exp.lhs->getType()!=exp.rhs->getType())
        return error(exp.loc,typeid(exp).name());
}

void Sclp::AstTypeCheck::visit(ReadStmtAst& exp){
    exp.target->accept(*this);

    auto expType = exp.target->getType();
    if (expType != dataType::Int && expType != dataType::Float)
        return error(exp.loc,typeid(exp).name());

    return;
}

void Sclp::AstTypeCheck::visit(WriteStmtAst& exp){
    exp.target->accept(*this);

    auto expType = exp.target->getType();
    if (expType != dataType::Int && expType != dataType::Float && expType != dataType::String)
        return error(exp.loc,typeid(exp).name());

    return;
}

void Sclp::AstTypeCheck::visit(IterationStmtAst& stmt){
    stmt.cond->accept(*this);
    if(stmt.cond->getType()!=dataType::Bool){
        error(stmt.cond->loc, "The condition in while should be a boolean");
    }
    stmt.stmt->accept(*this);
}

void Sclp::AstTypeCheck::visit(SelectionStmtAst& stmt){
    stmt.cond->accept(*this);
    if(stmt.cond->getType()!=dataType::Bool){
        error(stmt.cond->loc, "The condition in if should be a boolean");
    }
    stmt.stmt1->accept(*this);
    if(stmt.stmt2) stmt.stmt2->accept(*this);
}

void Sclp::AstTypeCheck::visit(SequenceStmtAst& stmt){
    for(auto &s:stmt.stmts){
        s->accept(*this);
    }
}

void Sclp::AstTypeCheck::visit(ReturnStmtAst& stmt){
    stmt.exp->accept(*this);
    if(stmt.exp->getType()!=current_procedure->return_type){
        error(stmt.loc,"Return type not same as procedure");
    }
    returnExists = true;
}

void Sclp::AstTypeCheck::visit(CallStmtAst& stmt){
    stmt.exp->accept(*this);
    if(stmt.exp->getType()!=dataType::Void){
        error(stmt.loc,"Return value of a procedure ignored");
    }
}