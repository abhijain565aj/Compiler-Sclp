#include "asm_builder.hh"
#include "rtl_registers.hh"
#include "program.hh"
#include <assert.h>
#include <algorithm>

size_t Sclp::AsmBuilder::emit(std::shared_ptr<AsmStmt> asm_){
    current_procedure->asm_code.push_back(asm_);
    return current_procedure->asm_code.size();
}

size_t Sclp::AsmBuilder::getCodeLineNo(){
    return current_procedure->asm_code.size();
}

void Sclp::AsmBuilder::fillGlobalOffsets(){
    int curr_offset = 0;
    for (auto it = current_program->globalSymTable->table.begin(); it !=current_program->globalSymTable->table.end(); it++){
        it->get()->start_offset = curr_offset;
        it->get()->end_offset = curr_offset = curr_offset + sizeofDType.at(it->get()->dType);
    }
};

void Sclp::AsmBuilder::fillOffsetsInSymTab(){
    int curr_offset = 0;
    for (auto it = current_procedure->localSymbols->table.begin(); it !=current_procedure->localSymbols->table.end(); it++){
        it->get()->start_offset = curr_offset;
        it->get()->end_offset = curr_offset = curr_offset + sizeofDType.at(it->get()->dType);
    }

    curr_offset = -8;
    for (auto it = current_procedure->paramList->table.begin(); it !=current_procedure->paramList->table.end(); it++){
        it->get()->end_offset = curr_offset;
        it->get()->start_offset = curr_offset = curr_offset - sizeofDType.at(it->get()->dType);
    }
}

void Sclp::AsmBuilder::getPrologue(){
    size_t start_lNo = getCodeLineNo();

    auto ra = std::make_shared<SpecialRegister>(specialRegType::ra);
    auto sp = std::make_shared<SpecialRegister>(specialRegType::sp);
    auto fp = std::make_shared<SpecialRegister>(specialRegType::fp);
    
    emit(std::make_shared<Sclp::LabelAsmStmt>(std::make_shared<Sclp::LabelAsmOpd>(current_procedure->name)));
    emit(std::make_shared<Sclp::MoveAsmStmt>(MoveAsmStmt::MoveType::sw, std::make_shared<RegisterOffsetAsmOpd>(sp, 0), std::make_shared<RegisterAsmOpd>(ra)));
    emit(std::make_shared<Sclp::MoveAsmStmt>(MoveAsmStmt::MoveType::sw, std::make_shared<RegisterOffsetAsmOpd>(sp, -4), std::make_shared<RegisterAsmOpd>(fp)));
    emit(std::make_shared<ComputeAsmStmt>(std::make_shared<RegisterAsmOpd>(fp), std::make_shared<RegisterAsmOpd>(sp), Sclp::AbOp::SUB, std::make_shared<IntConstAsmOpd>(4)));
    emit(std::make_shared<ComputeAsmStmt>(std::make_shared<RegisterAsmOpd>(sp), std::make_shared<RegisterAsmOpd>(sp), Sclp::AbOp::SUB, std::make_shared<IntConstAsmOpd>(8 + current_procedure->localSymbols->getMaxOffset())));

    size_t end_lNo = getCodeLineNo();
    current_procedure->prologue = std::make_shared<AsmStmtAnnotation>(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::getEpilogue(){
    size_t start_lNo = getCodeLineNo();

    auto ra = std::make_shared<SpecialRegister>(specialRegType::ra);
    auto sp = std::make_shared<SpecialRegister>(specialRegType::sp);
    auto fp = std::make_shared<SpecialRegister>(specialRegType::fp);

    emit(std::make_shared<Sclp::LabelAsmStmt>(std::make_shared<Sclp::LabelAsmOpd>("epilogue_"+current_procedure->name)));
    
    emit(std::make_shared<ComputeAsmStmt>(std::make_shared<RegisterAsmOpd>(sp), std::make_shared<RegisterAsmOpd>(sp), Sclp::AbOp::ADD, std::make_shared<IntConstAsmOpd>(8 + current_procedure->localSymbols->getMaxOffset())));
    emit(std::make_shared<Sclp::MoveAsmStmt>(MoveAsmStmt::MoveType::lw, std::make_shared<RegisterAsmOpd>(fp), std::make_shared<RegisterOffsetAsmOpd>(sp, -4)));
    emit(std::make_shared<Sclp::MoveAsmStmt>(MoveAsmStmt::MoveType::lw, std::make_shared<RegisterAsmOpd>(ra), std::make_shared<RegisterOffsetAsmOpd>(sp, 0)));
    emit(std::make_shared<Sclp::JumpRegAsmStmt>()); // jr $ra

    size_t end_lNo = getCodeLineNo();
    current_procedure->epilogue = std::make_shared<AsmStmtAnnotation>(start_lNo, end_lNo);
}

Sclp::AsmBuilder::AsmBuilder(std::shared_ptr<Program> p):current_program(p){
    fillGlobalOffsets();
    for(auto &proc : p->getSortedFuncDeclarations()){
        visit(proc->procedure);
    }
}

void Sclp::AsmBuilder::visit(std::shared_ptr<Procedure> p){
    current_procedure = p;

    fillOffsetsInSymTab();
    getPrologue();

    for(auto& stmt : p->rtl_code){
        stmt->accept(*this);
    }
    getEpilogue();
}

void Sclp::AsmBuilder::visit(RtlOpd&) {}

void Sclp::AsmBuilder::visit(ConstRtlOpd&) {}

void Sclp::AsmBuilder::visit(DoubleConstRtlOpd& opd) {
    opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::DoubleConstAsmOpd>(opd.value));
}

void Sclp::AsmBuilder::visit(IntConstRtlOpd& opd) {
    opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::IntConstAsmOpd>(opd.value));
}

void Sclp::AsmBuilder::visit(StrConstRtlOpd& opd) {
    opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::StrConstAsmOpd>(opd.str_num));
}

void Sclp::AsmBuilder::visit(LabelRtlOpd& opd) {
    opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::LabelAsmOpd>(opd.label));
};

void Sclp::AsmBuilder::visit(VariableRtlOpd& opd) {
    static const std::shared_ptr<Sclp::SpecialRegister> fp = std::make_shared<SpecialRegister>(specialRegType::fp);

    auto symEntry = current_procedure->localSymbols->lookup(opd.name);
    auto symEntryParam = current_procedure->paramList->lookup(opd.name);
    if (symEntry){
        opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::RegisterOffsetAsmOpd>(fp, - symEntry->end_offset));
    } else if(symEntryParam){
        opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::RegisterOffsetAsmOpd>(fp, - symEntryParam->end_offset));
    } else {
        opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::GlobalVarAsmOpd>( opd.name));
    }
};

void Sclp::AsmBuilder::visit(RegisterRtlOpd& opd) {
    opd.asm_an = Sclp::AsmOpdAnnotation(std::make_shared<Sclp::RegisterAsmOpd>(opd.reg));
};

void Sclp::AsmBuilder::visit(RtlStmt&) {};

void Sclp::AsmBuilder::visit(ComputeRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();

    if(stmt.lhs) stmt.lhs->accept(*this);
    if (stmt.rhs1) stmt.rhs1->accept(*this);
    if (stmt.rhs2) stmt.rhs2->accept(*this);

    size_t end_lNo = emit(std::make_shared<Sclp::ComputeAsmStmt>(stmt.lhs?stmt.lhs->asm_an.value:nullptr, stmt.rhs1?stmt.rhs1->asm_an.value:nullptr, stmt.op, stmt.rhs2?stmt.rhs2->asm_an.value:nullptr));
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(WriteRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    size_t end_lNo = emit(std::make_shared<Sclp::SyscallAsmStmt>());
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(ReadRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    size_t end_lNo = emit(std::make_shared<Sclp::SyscallAsmStmt>());
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(MoveRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();

    if(stmt.lhs) stmt.lhs->accept(*this);
    if(stmt.rhs1) stmt.rhs1->accept(*this);
    if(stmt.rhs2) stmt.rhs2->accept(*this);

    MoveAsmStmt::MoveType mt;

    static std::map<MoveRtlStmt::MoveType,MoveAsmStmt::MoveType> mp ={
        {MoveRtlStmt::MoveType::load, MoveAsmStmt::MoveType::lw},
        {MoveRtlStmt::MoveType::store, MoveAsmStmt::MoveType::sw},
        {MoveRtlStmt::MoveType::iLoad, MoveAsmStmt::MoveType::li},
        {MoveRtlStmt::MoveType::load_addr, MoveAsmStmt::MoveType::la},
        {MoveRtlStmt::MoveType::iLoadD, MoveAsmStmt::MoveType::liD},
        {MoveRtlStmt::MoveType::storeD, MoveAsmStmt::MoveType::sd},
        {MoveRtlStmt::MoveType::loadD, MoveAsmStmt::MoveType::ld},
        {MoveRtlStmt::MoveType::move,MoveAsmStmt::MoveType::move},
        {MoveRtlStmt::MoveType::moveD, MoveAsmStmt::MoveType::moveD},
        {MoveRtlStmt::MoveType::movt, MoveAsmStmt::MoveType::movt},
        {MoveRtlStmt::MoveType::movf, MoveAsmStmt::MoveType::movf},
    };

    mt = mp.at(stmt.type);
    emit(std::make_shared<Sclp::MoveAsmStmt>(mt, stmt.lhs?stmt.lhs->asm_an.value:nullptr, stmt.rhs1?stmt.rhs1->asm_an.value:nullptr, stmt.rhs2?stmt.rhs2->asm_an.value:nullptr));
    size_t end_lNo = getCodeLineNo();
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(StackOpRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();

    if(stmt.lhs) stmt.lhs->accept(*this);

    auto sp = std::make_shared<SpecialRegister>(specialRegType::sp);
    auto spr = std::make_shared<RegisterAsmOpd>(sp);

    if(stmt.type == StackOpRtlStmt::OpType::push){
        if (stmt.dType == dataType::Float){
            emit(std::make_shared<Sclp::MoveAsmStmt>(MoveAsmStmt::MoveType::sd, std::make_shared<RegisterOffsetAsmOpd>(sp, -4), stmt.lhs->asm_an.value));
            emit(std::make_shared<ComputeAsmStmt>(spr,spr, Sclp::AbOp::SUB, std::make_shared<IntConstAsmOpd>(8)));
        } else {
            emit(std::make_shared<Sclp::MoveAsmStmt>(MoveAsmStmt::MoveType::sw, std::make_shared<RegisterOffsetAsmOpd>(sp, 0), stmt.lhs->asm_an.value));
            emit(std::make_shared<ComputeAsmStmt>(spr,spr, Sclp::AbOp::SUB, std::make_shared<IntConstAsmOpd>(4)));
        }
    }else if(stmt.type == StackOpRtlStmt::OpType::pop){
        if (stmt.dType == dataType::Float){
            emit(std::make_shared<ComputeAsmStmt>(spr,spr, Sclp::AbOp::ADD, std::make_shared<IntConstAsmOpd>(8)));
        } else {
            emit(std::make_shared<ComputeAsmStmt>(spr,spr, Sclp::AbOp::ADD, std::make_shared<IntConstAsmOpd>(4)));
        }
    }

    size_t end_lNo = getCodeLineNo();
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(LabelRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    stmt.label->accept(*this);

    size_t end_lNo = emit(std::make_shared<Sclp::LabelAsmStmt>(stmt.label->asm_an.value));
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
};

void Sclp::AsmBuilder::visit(ControlFlowRtlStmt&) {};

void Sclp::AsmBuilder::visit(GotoRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    stmt.label->accept(*this);

    size_t end_lNo = emit(std::make_shared<Sclp::GotoAsmStmt>(stmt.label->asm_an.value));
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(IfGotoRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    stmt.label->accept(*this);
    stmt.var->accept(*this);

    size_t end_lNo = emit(std::make_shared<Sclp::IfGotoAsmStmt>(stmt.label->asm_an.value, stmt.var->asm_an.value));
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
}

void Sclp::AsmBuilder::visit(ReturnRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    emit(std::make_shared<GotoAsmStmt>(std::make_shared<LabelAsmOpd>("epilogue_"+current_procedure->name)));
    size_t end_lNo = getCodeLineNo();
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
};

void Sclp::AsmBuilder::visit(CallRtlStmt& stmt) {
    size_t start_lNo = getCodeLineNo();
    stmt.func->accept(*this);
    emit(std::make_shared<CallAsmStmt>(stmt.func->asm_an.value));
    size_t end_lNo = getCodeLineNo();
    stmt.asm_an = AsmStmtAnnotation(start_lNo, end_lNo);
};