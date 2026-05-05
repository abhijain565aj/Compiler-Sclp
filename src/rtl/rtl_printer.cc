#include <iomanip>
#include <fstream>
#include <ostream>
#include <string>
#include <memory>
#include <map>
#include <string>
#include <iostream>

#include "rtl.hh"
#include "rtl_registers.hh"
#include "rtl_printer.hh"

void Sclp::Register::print(std::ostream &) const{
}

void Sclp::NormalRegister::print(std::ostream &os) const{
    if (t == dataType::Int) {
        os << rtl_int_reg_names[rNo];
    } else if (t == dataType::Float) {
        os << rtl_float_reg_names[rNo];
    }
}

void Sclp::SpecialRegister::print(std::ostream &os) const{
    static std::map<specialRegType,std::string> mp = {
        {specialRegType::a0,"a0"},
        {specialRegType::f0,"f0"},
        {specialRegType::v1,"v1"},
        {specialRegType::zero,"zero"},
        {specialRegType::ra, "ra"},
        {specialRegType::fp, "fp"},
        {specialRegType::sp, "sp"}
    };
    os << mp.at(reg);
}

void Sclp::printRTL(std::shared_ptr<Program> program, std::string rtl_print_file){
    auto os = std::ofstream(rtl_print_file);
    if (!os.is_open()){
        std::cerr<<"Error: Cannot open file "<<rtl_print_file<<std::endl;
        exit(1);
    }
    os<<std::fixed;
    os<<std::setprecision(2);
    auto sp = program->getSortedProcedures();
    for(auto& procedure : sp){
        if(procedure->body.size() == 0) 
            continue;
        os<<"**PROCEDURE: "<<procedure->name<<std::endl;
        os<<"**BEGIN: RTL Statements"<<std::endl;
        for(auto&stmt:procedure->rtl_code){
            stmt->print(os);
            os << std::endl;
        }
        os<<"**END: RTL Statements"<<std::endl;
    }
}

// RTL Opds
void Sclp::DoubleConstRtlOpd::print(std::ostream& res) const{
    res<<value;
}

void Sclp::IntConstRtlOpd::print(std::ostream& res) const{
    res<<value;
}

void Sclp::StrConstRtlOpd::print(std::ostream& res) const{
    res<<"_str_"<<str_num;
}

void Sclp::LabelRtlOpd::print(std::ostream& res) const{
    res<<label;
}

void Sclp::VariableRtlOpd::print(std::ostream& res) const{
    res<<name;
}

void Sclp::RegisterRtlOpd::print(std::ostream& res) const{
    reg->print(res);
}

// RTL Stmts
void Sclp::MoveRtlStmt::print(std::ostream& res) const{
    static std::map<MoveType,std::string> mp = {
        {MoveType::iLoad,"iLoad"},
        {MoveType::iLoadD,"iLoad.d"},
        {MoveType::load,"load"},
        {MoveType::loadD,"load.d"},
        {MoveType::store,"store"},
        {MoveType::storeD,"store.d"},
        {MoveType::load_addr,"load_addr"},
        {MoveType::move,"move"},
        {MoveType::moveD,"move.d"},
        {MoveType::movt,"movt"},
        {MoveType::movf,"movf"}
    };
    res << "\t"<<mp.at(type);
    if(lhs){
        res<<":\t";
        lhs->print(res);
    }
    if(rhs1){
        res<<" <- ";
        rhs1->print(res);
    }
    if(rhs2){
        res<<" , ";
        rhs2->print(res);
    }
}

// RTL Stmts
void Sclp::StackOpRtlStmt::print(std::ostream& res) const{
    static std::map<OpType,std::string> mp = {
        {OpType::push,"push"},
        {OpType::pop,"pop"},
    };
    res << "\t"<<mp.at(type);
    if(lhs){
        res<<":\t";
        lhs->print(res);
    }
}

void Sclp::ComputeRtlStmt::print(std::ostream& res) const{
    res<<"\t"<<opToRtl.at(op);
    if(type == dataType::Float)res<<".d";
    res<<":\t";
    if(lhs){
        lhs->print(res);
        res<<" <- ";
    }
    rhs1->print(res);
    if(rhs2){
        res<<" , ";
        rhs2->print(res);
    }
}

void Sclp::WriteRtlStmt::print(std::ostream& res) const{
    res<<"\twrite";
}

void Sclp::ReadRtlStmt::print(std::ostream& res) const{
    res<<"\tread";
}

void Sclp::GotoRtlStmt::print(std::ostream& res) const{
    res<<"\tgoto:\tLabel";
    label->print(res);
}

void Sclp::LabelRtlStmt::print(std::ostream& res) const{
    res<<"\nLabel";
    label->print(res);
    res<<":";
}

void Sclp::IfGotoRtlStmt::print(std::ostream& res) const{
    res<<"\tbgtz:\t";
    var->print(res);
    res<<" , Label";
    label->print(res);
}

void Sclp::ReturnRtlStmt::print(std::ostream& res) const{
    res<<"\treturn ";
    var->print(res);
}

void Sclp::CallRtlStmt::print(std::ostream& res) const{
    res<<"\t";
    if(lhs!=std::nullopt){
        lhs.value()->print(res);
        res<<" = ";
    }
    res<<"call ";
    func->print(res);
}

void Sclp::ControlFlowRtlStmt::print(std::ostream&) const{

}


