#include "rtl.hh"
#include "rtl_registers.hh"
#include "rtl_builder.hh"
#include <memory>
#include <iostream>

Sclp::Register::~Register(){
}

void Sclp::RegisterAllocator::error(std::string msg){
    std::cerr<<"Error in Register Allocator: "<<msg<<std::endl;
    exit(1);
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::get_int_reg(size_t temp_no){
    for (size_t i=0; i<num_int_regs; i++){
        if (!int_regs[i].has_value()){
            int_regs[i] = temp_no;
            return std::make_shared<NormalRegister>(i, dataType::Int);
        }
    }
    error("out of int registers");
    return nullptr;
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::get_float_reg(size_t temp_no){
    for (size_t i=0; i<num_float_regs; i++){
        if (!float_regs[i].has_value()){
            float_regs[i] = temp_no;
            return std::make_shared<NormalRegister>(i, dataType::Float);
        }
    }
    error("out of float registers");
    return nullptr;
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::free_int_reg(size_t temp_no){
    for (size_t i=0; i<num_int_regs; i++){
        if (int_regs[i]==temp_no){
            int_regs[i] = std::nullopt;
            return std::make_shared<NormalRegister>(i, dataType::Int);
        }
    }
    error("freed register not found");
    return nullptr;
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::free_float_reg(size_t temp_no){
    for (size_t i=0; i<num_float_regs; i++){
        if (float_regs[i]==temp_no){
            float_regs[i] = std::nullopt;
            return std::make_shared<NormalRegister>(i, dataType::Float);
        }
    }
    error("freed register not found");
    return nullptr;
}

Sclp::RegisterAllocator::RegisterAllocator(Sclp::RtlBuilder* _builder): rtl_builder(_builder){
    int_regs.resize(num_int_regs, std::nullopt);
    float_regs.resize(num_float_regs, std::nullopt);
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::get_reg(size_t temp_no, dataType t){
    return (t==dataType::Float)?get_float_reg(temp_no):get_int_reg(temp_no);
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::free_reg(size_t temp_no, dataType t){
    return (t==dataType::Float)?free_float_reg(temp_no):free_int_reg(temp_no);
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::get_local_reg(dataType t){
    return get_reg(max_temps, t);
}

void Sclp::RegisterAllocator::free_all_local_regs(){
    for (size_t i=0; i<num_int_regs; i++){
        if (int_regs[i]==max_temps){
            int_regs[i] = std::nullopt;
        }
    }

    for (size_t i=0; i<num_float_regs; i++){
        if (float_regs[i]==max_temps){
            float_regs[i] = std::nullopt;
        }
    }
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::get_v0(){
    std::shared_ptr<NormalRegister> v0 = std::make_shared<NormalRegister>(0, dataType::Int);

    if (int_regs[0]!=std::nullopt){
        std::shared_ptr<NormalRegister> r = get_reg(int_regs[0].value(),dataType::Int);
        
        rtl_builder->emit(std::make_shared<Sclp::MoveRtlStmt>(std::make_shared<Sclp::RegisterRtlOpd>(r), std::make_shared<Sclp::RegisterRtlOpd>(v0)));
    }
    // assert(int_regs[0]==std::nullopt);

    int_regs[0] = max_temps+1; // occupied by syscall
    return v0;
}

std::shared_ptr<Sclp::NormalRegister> Sclp::RegisterAllocator::get_f12(){
    std::shared_ptr<NormalRegister> f12 = std::make_shared<NormalRegister>(5, dataType::Float);
    
    if (float_regs[5]!=std::nullopt){
        std::shared_ptr<NormalRegister> r = get_reg(float_regs[5].value(),dataType::Float);
        
        rtl_builder->emit(std::make_shared<Sclp::MoveRtlStmt>(std::make_shared<Sclp::RegisterRtlOpd>(r), std::make_shared<Sclp::RegisterRtlOpd>(f12)));
    }
    // assert(float_regs[5]==std::nullopt);

    float_regs[5] = max_temps+2; // occupied by syscall
    return f12;
}

void Sclp::RegisterAllocator::free_v0(){
    assert(int_regs[0] == max_temps+1);
    int_regs[0] = std::nullopt;
}

void Sclp::RegisterAllocator::free_f12(){
    assert(float_regs[5] == max_temps+2);
    float_regs[5] = std::nullopt;
}