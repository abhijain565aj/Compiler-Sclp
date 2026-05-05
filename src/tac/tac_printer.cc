#include <iomanip>
#include <fstream>
#include <ostream>
#include <string>
#include <memory>
#include "tac_printer.hh"
#include "tac.hh"

void Sclp::printTAC(std::shared_ptr<Sclp::Program> program, std::string tac_print_file){
    auto os = std::ofstream(tac_print_file);
    if (!os.is_open()){
        std::cerr<<"Error: Cannot open file "<<tac_print_file<<std::endl;
        exit(1);
    }

    os<<std::fixed;
    os<<std::setprecision(2);
    auto printProcedures = program->getSortedProcedures();
    for(auto& procedure: printProcedures){
        if(procedure->body.size() == 0) 
            continue;
        os<<"**PROCEDURE: "<<procedure->name<<std::endl;
        os<<"**BEGIN: Three Address Code Statements"<<std::endl;
        for(auto&stmt:procedure->tac_code){
            stmt->print(os);
            os << std::endl;
        }
        os<<"**END: Three Address Code Statements"<<std::endl;
    }
}    

void Sclp::DoubleConstTacOpd::print(std::ostream& res) const{
    res<<value;
    return;
}

void Sclp::IntConstTacOpd::print(std::ostream& res) const{
    res<<value;
    return;
}

void Sclp::StrConstTacOpd::print(std::ostream& res) const{
    res<<value;
    return;
}

void Sclp::LabelTacOpd::print(std::ostream& res) const{
    res<<"Label"<<label;
    return;
}

void Sclp::VariableTacOpd::print(std::ostream& res) const{
    res<<name;
    return;
}

void Sclp::TempraryTacOpd::print(std::ostream& res) const{
    res<<(isSTemp?"s":"")<<"temp"<<place;
    return;
}

const std::string TAB_SPACE = "\t";

void Sclp::AsgnTacStmt::print(std::ostream& res) const{
    res<<TAB_SPACE;
    lhs->print(res);
    res<<" = ";
    rhs->print(res);
    return;
}

void Sclp::ComputeTacStmt::print(std::ostream& res) const{
    res<<TAB_SPACE;
    lhs->print(res);
    res<<" = ";
    if(rhs2 && rhs1){
        rhs1->print(res);
        res <<" ";
        res<<opToString.at(type)<<" ";
        rhs2->print(res);
    }else {
        res<<opToString.at(type)<<" ";
        rhs1->print(res);
    }
    return;
}


void Sclp::IOTacStmt::print(std::ostream& res) const{
    res<<TAB_SPACE;
    if(op == Op::READ) res << "read ";
    else if(op == Op::WRITE) res << "write ";
    var->print(res);
    return;
}

void Sclp::GotoTacStmt::print(std::ostream& res) const{
    res << TAB_SPACE << "goto ";
    label->print(res);
    return;
}

void Sclp::LabelTacStmt::print(std::ostream& res) const{
    label->print(res);
    res<<":";
    return;
}

void Sclp::IfGotoTacStmt::print(std::ostream& res) const{
    res << TAB_SPACE << "if(";
    var->print(res); 
    res << ") goto ";
    label->print(res);
    return;
}

void Sclp::ReturnTacStmt::print(std::ostream& res) const{
    res << TAB_SPACE << "return ";
    var->print(res); 
    return;
}

void Sclp::CallTacStmt::print(std::ostream& res) const{
    res << TAB_SPACE;
    if(lhs!=std::nullopt){
        lhs.value()->print(res);
        res<<" = ";
    }
    func->print(res);
    res <<"(";
    for(size_t i=0;i < args.size();++i){
        if(i!=0) res<<", ";
        args[i]->print(res);
    }
    res << ")";
    return;
}