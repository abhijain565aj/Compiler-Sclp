#ifndef RTL_REGISTERS_HH
#define RTL_REGISTERS_HH

// #include "ast.hh"
#include "declarations.hh"
#include <string>
#include <vector>
#include <optional>
#include <assert.h>

namespace Sclp{

const size_t num_int_regs = 19;
const size_t num_float_regs = 15;

const std::vector<std::string> rtl_int_reg_names = {
    "v0", 
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"
};

const std::vector<std::string> rtl_float_reg_names = {
    "f2", "f4", "f6", "f8", "f10", "f12", "f14", "f16", "f18", "f20", "f22", "f24", "f26", "f28", "f30"
};

enum class specialRegType {a0, f0, v1, zero, ra, fp, sp};

class Register {
public:
    dataType t;
    virtual ~Register() = 0; 
    virtual void print(std::ostream &os) const = 0;
};

class NormalRegister : public Register{
public:
    size_t rNo;

    NormalRegister(size_t _rNo, dataType _t): rNo(_rNo) {
        t = _t;
    }
    ~NormalRegister() = default;
    void print(std::ostream &os) const override;

};

class SpecialRegister : public Register{
public:
    specialRegType reg;

    SpecialRegister(specialRegType _reg): reg(_reg){
        t = dataType::Int;
        if(reg == specialRegType::f0) t = dataType::Float;
    }
    ~SpecialRegister() = default;
    void print(std::ostream &os) const override;
};

class RegisterAllocator{
    Sclp::RtlBuilder* rtl_builder;
    std::vector<std::optional<size_t>> int_regs;
    std::vector<std::optional<size_t>> float_regs;

    static constexpr size_t max_temps = 10000;

    std::shared_ptr<NormalRegister> get_int_reg(size_t temp_no);
    std::shared_ptr<NormalRegister> get_float_reg(size_t temp_no);
    std::shared_ptr<NormalRegister> free_int_reg(size_t temp_no);
    std::shared_ptr<NormalRegister> free_float_reg(size_t temp_no);
    void error(std::string msg);

public:
    RegisterAllocator(Sclp::RtlBuilder* _builder);

    // apis for getting and freeing registers temps
    std::shared_ptr<NormalRegister> get_reg(size_t temp_no, dataType t);
    std::shared_ptr<NormalRegister> free_reg(size_t temp_no, dataType t);

    // apis for getting and freeing registers for single statement use (non temp things)
    std::shared_ptr<NormalRegister> get_local_reg(dataType t);
    void free_all_local_regs();
    std::shared_ptr<NormalRegister> get_v0();
    std::shared_ptr<NormalRegister> get_f12();
    void free_v0();
    void free_f12();
    
};

}

#endif