#include <iomanip>
#include <fstream>
#include <ostream>
#include <string>
#include <memory>
#include <map>
#include <string>
#include <iostream>

#include "asm.hh"
#include "rtl_registers.hh"
#include "asm_printer.hh"
#include "operator.hh"
#include "ast_printer.hh"

void Sclp::printSymTab(std::shared_ptr<Program> program, std::string symtab_print_file){
    auto os = std::ofstream(symtab_print_file);
    if (!os.is_open()){
        std::cerr<<"Error: Cannot open file "<<symtab_print_file<<std::endl;
        exit(1);
    }

    auto printTable = [&](std::vector<std::shared_ptr<SymbolEntry>>& table, bool offset){
        for(auto& entry:table){
            if(entry->sType==SymbolEntry::symbolType::STEMP && !offset) continue;
            os<<"\tName: "<<entry->name<<TYPE_NAME.at(entry->dType)<<" ";
            os<<"";
            os<<"Entity Type: VAR ";
            if(offset){
                os<<"Start Offset: "<<-entry->end_offset<<" "<<"End Offset: "<<-entry->start_offset;
            }else{
                os<<"(No offset assigned yet)";
            }
            os<<"\n";
        }
    };
    for(bool offset:{false,true}){
        os<<"\nGlobal Declarations:\n";
        printTable(program->globalSymTable->table, offset);
        
        auto sp = program->getSortedProcedures();
        for(auto& procedure : sp){
            os<<"\n**PROCEDURE: "<<procedure->name<<", "<<"Return Type:"<< TYPE_NAME.at(procedure->return_type)<<"\n";
            os<<"Formal Parameters\n";
            printTable(procedure->paramList->table, offset);          
            os<<"Local Declarartions\n";
            printTable(procedure->localSymbols->table, offset);          
        }
    }
}

void Sclp::printASM(std::shared_ptr<Program> program, std::string asm_print_file){
    auto os = std::ofstream(asm_print_file);
    if (!os.is_open()){
        std::cerr<<"Error: Cannot open file "<<asm_print_file<<std::endl;
        exit(1);
    }
    os<<std::fixed;
    os<<std::setprecision(2);

    os<<"\n";

    if (program->globalSymTable->table.size()!=0 || program->stringTable.size()!=0){
        os<<"\t.data\n";
        for (auto var: program->globalSymTable->table){
            os<<var->name<<":\t"<<((var->dType==dataType::Float)?".double 0.0":".word 0")<<"\n";
        }
        for (size_t i=0; i<program->stringTable.size(); i++){
            os<<"_str_"<<i<<":\t"<<".asciiz "<<program->stringTable[i]<<"\n";
        }
        os<<"\n";
    }
    
    auto sp = program->getSortedFuncDeclarations();
    for(auto& entry : sp){
        os<<"\t.text\n";
        os<<"\t.globl "<<entry->procedure->name<<"\n";
        for(auto&stmt:entry->procedure->asm_code){
            stmt->print(os);
            os << std::endl;
        }
    }
}

void Sclp::DoubleConstAsmOpd::print(std::ostream& res) const{
    res<<value;
}

void Sclp::IntConstAsmOpd::print(std::ostream& res) const{
    res<<value;
}

void Sclp::StrConstAsmOpd::print(std::ostream& res) const{
    res<<"_str_"<<strNum;
}

void Sclp::LabelAsmOpd::print(std::ostream& res) const{
    res<<label;
}

void Sclp::GlobalVarAsmOpd::print(std::ostream& res) const{
    res<<name;
}

void Sclp::RegisterAsmOpd::print(std::ostream& res) const{
    res<<"$";
    reg->print(res);
}

void Sclp::RegisterOffsetAsmOpd::print(std::ostream& res) const{
    res<<offset<<"($";
    reg->print(res);
    res<<")";
}

// RTL Stmts
void Sclp::MoveAsmStmt::print(std::ostream& res) const{
    static std::map<MoveType,std::string> mp = {
        {MoveType::lw, "lw"},
        {MoveType::sw, "sw"},
        {MoveType::li,"li"},
        {MoveType::la,"la"},
        {MoveType::liD,"li.d"},
        {MoveType::ld,"l.d"},
        {MoveType::sd,"s.d"},
        {MoveType::move,"move"},
        {MoveType::moveD,"mov.d"},
        {MoveType::movt,"movt"},
        {MoveType::movf,"movf"},
    };
    
    switch (type){
        case MoveType::li:
        case MoveType::lw:
        case MoveType::la:
        case MoveType::ld:
        case MoveType::liD:
        case MoveType::move:
        case MoveType::moveD:
            res << "\t"<<mp.at(type)<<" ";
            lhs->print(res);
            res << ", ";
            rhs1->print(res);
            break;
        case MoveType::sw:
        case MoveType::sd:
            res << "\t"<<mp.at(type)<<" ";
            rhs1->print(res);
            res << ", ";
            lhs->print(res);
            break;
        case MoveType::movt:
        case MoveType::movf:
            res << "\t"<<mp.at(type)<<" ";
            lhs->print(res);
            res << ", ";
            rhs1->print(res);
            res << ", ";
            rhs2->print(res);
            break;
        default: assert(false);
    }
}

void Sclp::ComputeAsmStmt::print(std::ostream& res) const{
    if (std::holds_alternative<BuOp>(op) && std::get<BuOp>(op)==BuOp::NOT){
        res<<"\txori ";
        lhs->print(res);
        res<<", ";
        rhs1->print(res);
        res<<", 1\n";
        return;        
    }
    res<<"\t";
    if(type == dataType::Float)res<<opToAsmFloat.at(op);
    else res<<opToAsm.at(op);
    res<<" ";
    if(lhs){
        lhs->print(res);
        res<<", ";
    }
    rhs1->print(res);
    if(rhs2){
        res<<", ";
        rhs2->print(res);
    }
}

void Sclp::SyscallAsmStmt::print(std::ostream& res) const{
    res<<"\tsyscall";
}

void Sclp::GotoAsmStmt::print(std::ostream& res) const{
    res<<"\tj ";
    label->print(res);
}

void Sclp::LabelAsmStmt::print(std::ostream& res) const{
    label->print(res);
    res<<":";
}

void Sclp::IfGotoAsmStmt::print(std::ostream& res) const{
    res<<"\tbgtz ";
    var->print(res);
    res<<" ,";
    label->print(res);
}

void Sclp::JumpRegAsmStmt::print(std::ostream& res) const{
    res<<"\tjr ";
    reg->print(res);
}

void Sclp::CallAsmStmt::print(std::ostream& res) const{
    res<<"\t";
    res<<"jal ";
    func->print(res);
}

void Sclp::ControlFlowAsmStmt::print(std::ostream&) const{

}