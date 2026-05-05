#include "rtl_builder.hh"
#include "rtl_registers.hh"
#include "program.hh"
#include <assert.h>
#include <algorithm>

size_t Sclp::RtlBuilder::emit(std::shared_ptr<RtlStmt> rtl){
    current_procedure->rtl_code.push_back(rtl);
    return current_procedure->rtl_code.size();
}

size_t Sclp::RtlBuilder::getCodeLineNo(){
    return current_procedure->rtl_code.size();
}

Sclp::RtlBuilder::RtlBuilder(std::shared_ptr<Program> p){
    program = p;
    for(auto &proc : p->procedures){
        visit(proc);
    }
    program = nullptr;
}

void Sclp::RtlBuilder::visit(std::shared_ptr<Procedure> p){
    current_procedure = p;
    reg_allocator = std::make_shared<RegisterAllocator>(this);
    for(auto& stmt : p->tac_code){
        stmt->accept(*this);
        reg_allocator->free_all_local_regs();
    }
    current_procedure = nullptr;
}

void Sclp::RtlBuilder::visit(Sclp::TacOpd&){}

void Sclp::RtlBuilder::visit(Sclp::ConstTacOpd&){}

void Sclp::RtlBuilder::visit(Sclp::DoubleConstTacOpd& opd){
    opd.rtl_an = Sclp::RtlOpdAnnotation(std::make_shared<Sclp::DoubleConstRtlOpd>(opd.value));
}

void Sclp::RtlBuilder::visit(Sclp::IntConstTacOpd& opd){
    opd.rtl_an = Sclp::RtlOpdAnnotation(std::make_shared<Sclp::IntConstRtlOpd>(opd.value));
}

void Sclp::RtlBuilder::visit(Sclp::StrConstTacOpd& opd){
    opd.rtl_an = Sclp::RtlOpdAnnotation(std::make_shared<Sclp::StrConstRtlOpd>(program->insertString(opd.value)));
}

void Sclp::RtlBuilder::visit(Sclp::LabelTacOpd& opd){
    opd.rtl_an = Sclp::RtlOpdAnnotation(std::make_shared<Sclp::LabelRtlOpd>(opd.label));
}
void Sclp::RtlBuilder::visit(Sclp::VarTempTacOpd&){}

void Sclp::RtlBuilder::visit(Sclp::TempraryTacOpd& opd){
    // temps dealt in the statements themselves 
    if (opd.isSTemp) opd.rtl_an = Sclp::RtlOpdAnnotation(std::make_shared<Sclp::VariableRtlOpd>("stemp"+std::to_string(opd.place)));
}

void Sclp::RtlBuilder::visit(Sclp::VariableTacOpd& opd){
    opd.rtl_an = Sclp::RtlOpdAnnotation(std::make_shared<Sclp::VariableRtlOpd>(opd.name));
}


void Sclp::RtlBuilder::visit(Sclp::TacStmt&){}

void Sclp::RtlBuilder::visit(Sclp::AsgnTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.rhs->accept(*this);
    stmt.lhs->accept(*this);
    
    size_t end_lNo;
    // case: v = t
    if (stmt.lhs->is_var() && stmt.rhs->is_temp()){ 
        auto rtemp = std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.rhs);

        std::shared_ptr<Sclp::RegisterRtlOpd> rhs_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(rtemp->place,rtemp->type));
        end_lNo = emit(std::make_shared<Sclp::MoveRtlStmt>(stmt.lhs->rtl_an.value, rhs_reg));
    }
    // case: t = v or t = i
    else if (stmt.lhs->is_temp()){
        assert(!stmt.rhs->is_temp());

        auto ltemp = std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.lhs);
        std::shared_ptr<Sclp::RegisterRtlOpd> lhs_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_reg(ltemp->place, ltemp->type));
        end_lNo = emit(std::make_shared<Sclp::MoveRtlStmt>(lhs_reg, stmt.rhs->rtl_an.value));
    } 
    // case: v = v or v = i
    else {
        assert(stmt.lhs->is_var() && !stmt.rhs->is_temp());

        std::shared_ptr<Sclp::RegisterRtlOpd> tempReg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.rhs->type));

        emit(std::make_shared<Sclp::MoveRtlStmt>(tempReg, stmt.rhs->rtl_an.value));
        end_lNo = emit(std::make_shared<Sclp::MoveRtlStmt>(stmt.lhs->rtl_an.value, tempReg));
    }
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::handleFloatRbOp(Sclp::ComputeTacStmt &stmt){
    RbOp type = std::get<RbOp>(stmt.type);
    size_t start_lNo = getCodeLineNo();
    static std::map<RbOp,RbOp> mp = {
        {RbOp::EQ,RbOp::EQ},
        {RbOp::NEQ,RbOp::EQ},
        {RbOp::LT,RbOp::LT},
        {RbOp::GTE,RbOp::LT},
        {RbOp::LTE,RbOp::LTE},
        {RbOp::GT,RbOp::LTE},
    };

    stmt.rhs1->accept(*this);
    stmt.rhs2->accept(*this);
    stmt.lhs->accept(*this);

    std::shared_ptr<Sclp::RegisterRtlOpd> rhs_result_reg, rhs1_reg, rhs2_reg;

    if (!stmt.rhs1->is_temp()){
        rhs1_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.rhs1->type));
        emit(std::make_shared<Sclp::MoveRtlStmt>(rhs1_reg, stmt.rhs1->rtl_an.value));
    }
    auto v0 = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(dataType::Int));
    if (stmt.lhs->is_var()){
        rhs_result_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.lhs->type));
    } else if (stmt.lhs->is_temp()){
        rhs_result_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.lhs)->place, stmt.lhs->type));
    } 

    if (!stmt.rhs2->is_temp()){
        rhs2_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.rhs2->type));
        emit(std::make_shared<Sclp::MoveRtlStmt>(rhs2_reg, stmt.rhs2->rtl_an.value));
    }

    if (stmt.rhs1->is_temp()){
        rhs1_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.rhs1)->place, stmt.rhs1->type));
    }

    if (stmt.rhs2->is_temp()) {
        rhs2_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.rhs2)->place, stmt.rhs2->type));
    }   

    emit(std::make_shared<ComputeRtlStmt>(nullptr, rhs1_reg, mp.at(type), rhs2_reg));
    auto zero = std::make_shared<RegisterRtlOpd>(std::make_shared<SpecialRegister>(specialRegType::zero));
    emit(std::make_shared<MoveRtlStmt>(v0,std::make_shared<IntConstRtlOpd>(1))); //v0 <- 1
    emit(std::make_shared<MoveRtlStmt>(rhs_result_reg,zero)); //t0 <- zero
    if(mp.at(type) != type){
        emit(std::make_shared<MoveRtlStmt>(rhs_result_reg,v0,std::make_shared<IntConstRtlOpd>(0),MoveRtlStmt::MoveType::movf)); //movf
    }else{
        emit(std::make_shared<MoveRtlStmt>(rhs_result_reg,v0,std::make_shared<IntConstRtlOpd>(0),MoveRtlStmt::MoveType::movt)); //movt
    }
    if (stmt.lhs->is_var()){
        emit(std::make_shared<Sclp::MoveRtlStmt>(stmt.lhs->rtl_an.value, rhs_result_reg));
    }

    size_t end_lNo = getCodeLineNo();
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::ComputeTacStmt &stmt){
    if(std::holds_alternative<RbOp>(stmt.type) && stmt.rhs1->type == dataType::Float){
        handleFloatRbOp(stmt);
        return;
    }
    size_t start_lNo = getCodeLineNo();

    stmt.rhs1->accept(*this);
    if (stmt.rhs2) stmt.rhs2->accept(*this);
    stmt.lhs->accept(*this);

    std::shared_ptr<Sclp::RegisterRtlOpd> rhs_result_reg, rhs1_reg, rhs2_reg;

    if (!stmt.rhs1->is_temp()){
        rhs1_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.rhs1->type));
        emit(std::make_shared<Sclp::MoveRtlStmt>(rhs1_reg, stmt.rhs1->rtl_an.value));
    }

    if (stmt.lhs->is_var()){
        rhs_result_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.lhs->type));
    } else if (stmt.lhs->is_temp()){
        rhs_result_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.lhs)->place, stmt.lhs->type));
    } 

    if (stmt.rhs2 && !stmt.rhs2->is_temp()){
        rhs2_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.rhs2->type));
        emit(std::make_shared<Sclp::MoveRtlStmt>(rhs2_reg, stmt.rhs2->rtl_an.value));
    }

    if (stmt.rhs1->is_temp()){
        rhs1_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.rhs1)->place, stmt.rhs1->type));
    }

    if (stmt.rhs2 && stmt.rhs2->is_temp()) {
        rhs2_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.rhs2)->place, stmt.rhs2->type));
    }   

    emit(std::make_shared<Sclp::ComputeRtlStmt>(rhs_result_reg, rhs1_reg, stmt.type, rhs2_reg));
    
    if (stmt.lhs->is_var()){
        emit(std::make_shared<Sclp::MoveRtlStmt>(stmt.lhs->rtl_an.value, rhs_result_reg));
    }

    size_t end_lNo = getCodeLineNo();
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::IOTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.var->accept(*this);

    size_t end_lNo;

    if (stmt.op == Sclp::IOTacStmt::Op::READ){
        std::shared_ptr<NormalRegister> v0 = reg_allocator->get_v0();

        std::shared_ptr<Register> result_reg;
        if(stmt.var->type == dataType::Float) result_reg = std::make_shared<SpecialRegister>(specialRegType::f0);
        else result_reg = v0;

        emit(std::make_shared<Sclp::MoveRtlStmt>(std::make_shared<RegisterRtlOpd>(v0), Sclp::ReadRtlStmt::getSyscallNo(stmt.var->type)));
        emit(std::make_shared<Sclp::ReadRtlStmt>());
        end_lNo = emit(std::make_shared<Sclp::MoveRtlStmt>(stmt.var->rtl_an.value,std::make_shared<Sclp::RegisterRtlOpd>(result_reg)));

        reg_allocator->free_v0();
    } else {
        std::shared_ptr<NormalRegister> v0 = reg_allocator->get_v0();
        
        std::shared_ptr<RtlOpd> write_addr; // we could be printing from a temp
        if (stmt.var->is_temp()){
            write_addr = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.var)->place,stmt.var->type));
        } else {
            write_addr = stmt.var->rtl_an.value;
        }

        emit(std::make_shared<Sclp::MoveRtlStmt>(std::make_shared<RegisterRtlOpd>(v0), Sclp::WriteRtlStmt::getSyscallNo(stmt.var->type)));
        if(stmt.var->type == dataType::Float){
            emit(std::make_shared<Sclp::MoveRtlStmt>(std::make_shared<RegisterRtlOpd>(reg_allocator->get_f12()), write_addr));
            reg_allocator->free_f12();
        }else{
            emit(std::make_shared<Sclp::MoveRtlStmt>(std::make_shared<RegisterRtlOpd>(std::make_shared<SpecialRegister>(Sclp::specialRegType::a0)), write_addr));
        }
        end_lNo = emit(std::make_shared<Sclp::WriteRtlStmt>());
        reg_allocator->free_v0();
    }
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::GotoTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.label->accept(*this);

    size_t end_lNo = emit(std::make_shared<Sclp::GotoRtlStmt>(stmt.label->rtl_an.value));
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::LabelTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.label->accept(*this);

    size_t end_lNo = emit(std::make_shared<Sclp::LabelRtlStmt>(stmt.label->rtl_an.value));
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::IfGotoTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.label->accept(*this);
    stmt.var->accept(*this);

    std::shared_ptr<RegisterRtlOpd> cond_reg;
    if (stmt.var->is_var()) {
        cond_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(stmt.var->type));
        emit(std::make_shared<Sclp::MoveRtlStmt>(cond_reg, stmt.var->rtl_an.value));
    } else if (stmt.var->is_temp()){
        cond_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.var)->place, stmt.var->type));
    }
    size_t end_lNo = emit(std::make_shared<Sclp::IfGotoRtlStmt>(stmt.label->rtl_an.value, cond_reg));
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::CallTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    auto revArgs = stmt.args;
    reverse(revArgs.begin(),revArgs.end());
    for(auto& arg:revArgs){
        arg->accept(*this);
        std::shared_ptr<RegisterRtlOpd> reg;
        if (!arg->is_temp()){
            reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_local_reg(arg->type));
            emit(std::make_shared<Sclp::MoveRtlStmt>(reg,arg->rtl_an.value)); //load
        }else{
            reg = std::make_shared<RegisterRtlOpd>(reg_allocator->free_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(arg)->place, arg->type));
        }
        emit(std::make_shared<StackOpRtlStmt>(reg, reg->reg->t, StackOpRtlStmt::OpType::push));

        reg_allocator->free_all_local_regs();
    }
    if(stmt.lhs == std::nullopt){
        emit(std::make_shared<CallRtlStmt>(std::nullopt,std::make_shared<VariableRtlOpd>(stmt.func->name)));
        for(size_t i=0; i<stmt.args.size(); ++i){
            emit(std::make_shared<StackOpRtlStmt>(nullptr, stmt.args[i]->type, StackOpRtlStmt::OpType::pop));
        }
    }else{
        std::shared_ptr<RegisterRtlOpd> result_reg = std::make_shared<RegisterRtlOpd>(reg_allocator->get_reg(std::dynamic_pointer_cast<Sclp::TempraryTacOpd>(stmt.lhs.value())->place, stmt.lhs.value()->type));
        specialRegType t = (stmt.lhs.value()->type == dataType::Float) ? specialRegType::f0 : specialRegType::v1;
        auto v1 = std::make_shared<RegisterRtlOpd>(std::make_shared<SpecialRegister>(t));\
        emit(std::make_shared<CallRtlStmt>(v1,std::make_shared<VariableRtlOpd>(stmt.func->name)));
        for(size_t i=0; i<stmt.args.size(); ++i){
            emit(std::make_shared<StackOpRtlStmt>(nullptr, stmt.args[i]->type, StackOpRtlStmt::OpType::pop));
        }
        emit(std::make_shared<MoveRtlStmt>(result_reg,v1));
    }
    size_t end_lNo = getCodeLineNo();
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::RtlBuilder::visit(Sclp::ReturnTacStmt& stmt){
    size_t start_lNo = getCodeLineNo();
    stmt.var->accept(*this);
    specialRegType t = (stmt.var->type == dataType::Float) ? specialRegType::f0 : specialRegType::v1;
    auto v1 = std::make_shared<RegisterRtlOpd>(std::make_shared<SpecialRegister>(t));
    emit(std::make_shared<MoveRtlStmt>(v1,stmt.var->rtl_an.value));
    size_t end_lNo = emit(std::make_shared<ReturnRtlStmt>(v1));
    stmt.rtl_an = RtlStmtAnnotation(start_lNo, end_lNo);
}