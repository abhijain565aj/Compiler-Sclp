#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "tac_builder.hh"


std::shared_ptr<Sclp::TempraryTacOpd> Sclp::TacBuilder::getNewTemp(dataType type){
    return std::make_shared<Sclp::TempraryTacOpd>(type, false);
}

std::shared_ptr<Sclp::TempraryTacOpd> Sclp::TacBuilder::getNewSTemp(dataType type, bool goesAtStart){
    auto sTemp = std::make_shared<Sclp::TempraryTacOpd>(type, true);
    if (!goesAtStart)
        proc->localSymbols->insert(std::make_shared<SymbolEntry>("stemp"+std::to_string(sTemp->place), type, SymbolEntry::symbolType::STEMP));
    else
        proc->localSymbols->insertAtStart(std::make_shared<SymbolEntry>("stemp"+std::to_string(sTemp->place), type, SymbolEntry::symbolType::RETURN_STEMP));
    return sTemp;
}

std::shared_ptr<Sclp::LabelTacOpd> Sclp::TacBuilder::getNewLabel(){
    return std::make_shared<Sclp::LabelTacOpd>();
}

size_t Sclp::TacBuilder::emit(std::shared_ptr<TacStmt> tac){
    proc->tac_code.push_back(tac);
    return proc->tac_code.size();
}

void Sclp::TacBuilder::put(std::shared_ptr<TacStmt> tac, size_t pos){
    proc->tac_code.at(pos) = tac;
}

size_t Sclp::TacBuilder::getCodeLineNo(){
    return proc->tac_code.size();
}

Sclp::TacBuilder::TacBuilder(std::shared_ptr<Program> p){
    Sclp::LabelTacOpd::reset();
    for(auto& func : p->funcDeclarations->table){
        if(func->getReturnType()!=dataType::Void){
            returnLabel[func->procedure] = getNewLabel();
        }
    }
    auto sp = p->getSortedFuncDeclarations();
    for (auto &func: sp){
        proc = func->procedure;
        proc->accept(*this);
    }
}

std::shared_ptr<Sclp::LabelTacOpd> Sclp::TacBuilder::getReturnLabel(){
    return returnLabel.at(proc);
}

void Sclp::TacBuilder::visit(Procedure& p){
    Sclp::TempraryTacOpd::reset();
    if(p.return_type!=dataType::Void){
        return_s0 = getNewSTemp(p.return_type, true);
    }
    for (auto &stmt: p.body){
        stmt->accept(*this);
    }
    if(p.return_type!=dataType::Void){
        emit(std::make_shared<LabelTacStmt>(getReturnLabel()));
        emit(std::make_shared<ReturnTacStmt>(return_s0));
    }
}

void Sclp::TacBuilder::visit(Ast&){
}

void Sclp::TacBuilder::visit(StatementAst&){
}

void Sclp::TacBuilder::visit(ExpressionAst&){

}

void Sclp::TacBuilder::visit(BaseExprAst&){
}

void Sclp::TacBuilder::visit(UnaryExprAst&){
}

void Sclp::TacBuilder::visit(BinaryExprAst&){
}

void Sclp::TacBuilder::visit(TernaryExprAst&){
}

void Sclp::TacBuilder::visit(FunctionCallAst& exp){
    size_t start_lNo = getCodeLineNo();
    std::vector<std::shared_ptr<TacOpd>> args;
    std::optional<std::shared_ptr<TempraryTacOpd>> t0 = std::nullopt;
    if(exp.getType()!=dataType::Void) t0 = getNewTemp(exp.getType());
    for(auto& e : exp.args){
        e->accept(*this);
        args.push_back(e->tac_an.getPV());
    }
    emit(std::make_shared<CallTacStmt>(t0,std::make_shared<VariableTacOpd>(exp.name,exp.getType()),args));
    size_t end_lNo = getCodeLineNo();
    exp.tac_an = TacAnnotation(start_lNo,end_lNo,t0,std::nullopt);
}

void Sclp::TacBuilder::visit(NameExprAst& exp){
    Sclp::TacAnnotation::PLACE p= std::make_shared<Sclp::VariableTacOpd>(exp.name, exp.getType());
    exp.tac_an = TacAnnotation(getCodeLineNo(), getCodeLineNo(), p, std::nullopt);
}

void Sclp::TacBuilder::visit(NumberExprAst<int>& exp){
    Sclp::TacAnnotation::VALUE v= std::make_shared<Sclp::IntConstTacOpd>(exp.value);
    exp.tac_an = TacAnnotation(getCodeLineNo(), getCodeLineNo(), std::nullopt, v);
}

void Sclp::TacBuilder::visit(NumberExprAst<double>& exp){
    Sclp::TacAnnotation::VALUE v= std::make_shared<Sclp::DoubleConstTacOpd>(exp.value);
    exp.tac_an = TacAnnotation(getCodeLineNo(), getCodeLineNo(), std::nullopt, v);
}

void Sclp::TacBuilder::visit(StringExprAst& exp){
    Sclp::TacAnnotation::VALUE v= std::make_shared<Sclp::StrConstTacOpd>(exp.value);
    exp.tac_an = TacAnnotation(getCodeLineNo(), getCodeLineNo(), std::nullopt, v);
}

void Sclp::TacBuilder::visit(ArithmeticUnaryExprAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.operand->accept(*this);
    
    std::shared_ptr<Sclp::TempraryTacOpd> temp = getNewTemp(exp.getType());
    std::shared_ptr<Sclp::ComputeTacStmt> stmt = std::make_shared<Sclp::ComputeTacStmt>(temp, exp.op, exp.operand->tac_an.getPV());
    size_t end_lNo = emit(stmt);

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, temp, std::nullopt);
}

void Sclp::TacBuilder::visit(BooleanUnaryExprAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.operand->accept(*this);
    
    std::shared_ptr<Sclp::TempraryTacOpd> temp = getNewTemp(exp.getType());
    std::shared_ptr<Sclp::ComputeTacStmt> stmt = std::make_shared<Sclp::ComputeTacStmt>(temp, exp.op, exp.operand->tac_an.getPV());
    size_t end_lNo = emit(stmt);

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, temp, std::nullopt);
}

void Sclp::TacBuilder::visit(ArithmeticBinaryExprAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);
    
    std::shared_ptr<Sclp::TempraryTacOpd> temp = getNewTemp(exp.getType());
    std::shared_ptr<Sclp::ComputeTacStmt> stmt = std::make_shared<Sclp::ComputeTacStmt>(temp, exp.lhs->tac_an.getPV(), exp.op, exp.rhs->tac_an.getPV());
    size_t end_lNo = emit(stmt);

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, temp, std::nullopt);
}

void Sclp::TacBuilder::visit(BooleanBinaryExprAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);
    
    std::shared_ptr<Sclp::TempraryTacOpd> temp = getNewTemp(exp.getType());
    std::shared_ptr<Sclp::ComputeTacStmt> stmt = std::make_shared<Sclp::ComputeTacStmt>(temp, exp.lhs->tac_an.getPV(), exp.op, exp.rhs->tac_an.getPV());
    size_t end_lNo = emit(stmt);

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, temp, std::nullopt);
}

void Sclp::TacBuilder::visit(RelationalBinaryExprAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.lhs->accept(*this);
    exp.rhs->accept(*this);
    
    std::shared_ptr<Sclp::TempraryTacOpd> temp = getNewTemp(exp.getType());
    std::shared_ptr<Sclp::ComputeTacStmt> stmt = std::make_shared<Sclp::ComputeTacStmt>(temp, exp.lhs->tac_an.getPV(), exp.op, exp.rhs->tac_an.getPV());
    size_t end_lNo = emit(stmt);

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, temp, std::nullopt);
}

void Sclp::TacBuilder::visit(ConditionalExprAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.expr1->accept(*this);
    auto l1 = getNewLabel();
    auto l2 = getNewLabel();
    auto t2 = getNewSTemp(exp.getType());

    size_t s1 = emit(nullptr);
    size_t s2 = emit(nullptr);
    exp.expr2->accept(*this);
    size_t s3 = emit(nullptr);
    size_t s4 = emit(nullptr);
    size_t s5 = emit(nullptr);
    exp.expr3->accept(*this);

    auto t1 = getNewTemp(dataType::Bool);

    emit(std::make_shared<Sclp::AsgnTacStmt>(t2, exp.expr3->tac_an.getPV()));
    size_t end_lNo = emit(std::make_shared<LabelTacStmt>(l2));

    put(std::make_shared<Sclp::ComputeTacStmt>(t1, Sclp::BuOp::NOT, exp.expr1->tac_an.getPV()), s1-1);
    put(std::make_shared<Sclp::IfGotoTacStmt>(t1, l1), s2-1);
    put(std::make_shared<Sclp::AsgnTacStmt>(t2, exp.expr2->tac_an.getPV()), s3-1);
    put(std::make_shared<GotoTacStmt>(l2), s4-1);
    put(std::make_shared<LabelTacStmt>(l1), s5-1);

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, t2, std::nullopt);    
}

void Sclp::TacBuilder::visit(AssignmentStmtAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.rhs->accept(*this);
    exp.lhs->accept(*this);
    size_t end_lNo = emit(std::make_shared<Sclp::AsgnTacStmt>(exp.lhs->tac_an.getPV(), exp.rhs->tac_an.getPV()));

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, std::nullopt, std::nullopt);
}

void Sclp::TacBuilder::visit(ReadStmtAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.target->accept(*this);
    size_t end_lNo = emit(std::make_shared<Sclp::IOTacStmt>(Sclp::IOTacStmt::Op::READ, exp.target->tac_an.getPV()));

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, std::nullopt, std::nullopt);
}

void Sclp::TacBuilder::visit(WriteStmtAst& exp){
    size_t start_lNo = getCodeLineNo();
    exp.target->accept(*this);
    size_t end_lNo = emit(std::make_shared<Sclp::IOTacStmt>(Sclp::IOTacStmt::Op::WRITE, exp.target->tac_an.getPV()));

    exp.tac_an = TacAnnotation(start_lNo, end_lNo, std::nullopt, std::nullopt);
}

void Sclp::TacBuilder::visit(IterationStmtAst& stmt){
    size_t start_lNo = getCodeLineNo();
    if(stmt.type == IterationStmtAst::Type::DO_WHILE){
        auto s0 = emit(nullptr);
        stmt.stmt->accept(*this);
        stmt.cond->accept(*this);
        auto l0 = getNewLabel();
        put(std::make_shared<LabelTacStmt>(l0),s0-1);
        emit(std::make_shared<Sclp::IfGotoTacStmt>(stmt.cond->tac_an.getPV(),l0));
    }else{
        size_t s0 = emit(nullptr);
        stmt.cond->accept(*this);
        
        size_t s1 = emit(nullptr); //t1 = !cond
        size_t s2 = emit(nullptr); //if goto
        
        stmt.stmt->accept(*this);
        auto l0 = getNewLabel();
        put(std::make_shared<LabelTacStmt>(l0),s0-1);

        auto l1 = getNewLabel();
        auto t0 = getNewTemp(dataType::Bool);

        put(std::make_shared<Sclp::ComputeTacStmt>(t0,Sclp::BuOp::NOT,stmt.cond->tac_an.getPV()),s1-1);
        put(std::make_shared<Sclp::IfGotoTacStmt>(t0,l1),s2-1);

        emit(std::make_shared<GotoTacStmt>(l0));
        emit(std::make_shared<LabelTacStmt>(l1));
    }
    size_t end_lNo = getCodeLineNo();
    stmt.tac_an = TacAnnotation(start_lNo, end_lNo, std::nullopt, std::nullopt);
}

void Sclp::TacBuilder::visit(SelectionStmtAst& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.cond->accept(*this);
    
    size_t s1 = emit(nullptr); //t1 = !cond
    size_t s2 = emit(nullptr); //if goto

    stmt.stmt1->accept(*this);
    auto t0 = getNewTemp(dataType::Bool);
    put(std::make_shared<Sclp::ComputeTacStmt>(t0,Sclp::BuOp::NOT,stmt.cond->tac_an.getPV()),s1-1);

    bool if_else = stmt.type==Sclp::SelectionStmtAst::Type::IF_ELSE;

    std::shared_ptr<LabelTacOpd> l0,l1;
    l0 = getNewLabel();
    l1 = if_else ? getNewLabel() : l0;
    
    put(std::make_shared<Sclp::IfGotoTacStmt>(t0,l1),s2-1);
    emit(std::make_shared<Sclp::GotoTacStmt>(l0));
    if(if_else){
        emit(std::make_shared<Sclp::LabelTacStmt>(l1));
        stmt.stmt2->accept(*this);
    }

    emit(std::make_shared<Sclp::LabelTacStmt>(l0));
    size_t end_lNo = getCodeLineNo();
    stmt.tac_an = TacAnnotation(start_lNo, end_lNo, std::nullopt, std::nullopt);
}

void Sclp::TacBuilder::visit(SequenceStmtAst& stmt){
    size_t start_lNo = getCodeLineNo();
    for(auto &s: stmt.stmts){
        s->accept(*this);
    }
    size_t end_lNo = getCodeLineNo();

    stmt.tac_an = TacAnnotation(start_lNo, end_lNo, std::nullopt, std::nullopt);
}

void Sclp::TacBuilder::visit(CallStmtAst& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.exp->accept(*this);
    size_t end_lNo = getCodeLineNo();
    stmt.tac_an = TacAnnotation(start_lNo,end_lNo,std::nullopt,std::nullopt);
}

void Sclp::TacBuilder::visit(ReturnStmtAst& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.exp->accept(*this);
    emit(std::make_shared<AsgnTacStmt>(return_s0,stmt.exp->tac_an.getPV()));
    emit(std::make_shared<GotoTacStmt>(getReturnLabel()));
    size_t end_lNo = getCodeLineNo();
    stmt.tac_an = TacAnnotation(start_lNo,end_lNo,std::nullopt,std::nullopt);
}