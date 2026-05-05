#ifndef ASM_HH
#define ASM_HH

#include <string>
#include <memory>
#include <variant>
#include <ostream>
#include <map>
#include "operator.hh"
#include "rtl_registers.hh"

namespace Sclp{

class AsmOpd{
public:
    virtual void print(std::ostream& os) const = 0;
    virtual ~AsmOpd() = default;
};

class ConstAsmOpd : public AsmOpd{
    
};

class DoubleConstAsmOpd : public ConstAsmOpd{
public:
    double value;
    DoubleConstAsmOpd(double _value):value(_value){}
    void print(std::ostream& os) const override;
};

class IntConstAsmOpd : public ConstAsmOpd{
public:
    int value;
    IntConstAsmOpd(int _value):value(_value){}
    void print(std::ostream& os) const override;
};

class StrConstAsmOpd : public ConstAsmOpd{
public:
    int strNum;
    StrConstAsmOpd(int _strNum): strNum(_strNum){}

    void print(std::ostream& os) const override;
};

class LabelAsmOpd : public AsmOpd{
public:
    std::string label;
    LabelAsmOpd(int _label):label("Label"+std::to_string(_label)){}
    LabelAsmOpd(std::string _label):label(_label){}
    void print(std::ostream& os) const override;
};

class GlobalVarAsmOpd : public AsmOpd{
public:
    std::string name;
    GlobalVarAsmOpd(std::string _name):name(_name){}
    void print(std::ostream& os) const override;
};

class RegisterAsmOpd : public AsmOpd{
public:
    std::shared_ptr<Register> reg;
    RegisterAsmOpd(std::shared_ptr<Register> _reg):reg(_reg){}
    void print(std::ostream& os) const override;
};

class RegisterOffsetAsmOpd : public AsmOpd{
public:
    std::shared_ptr<Register> reg;
    int offset;
    
    RegisterOffsetAsmOpd(std::shared_ptr<Register> _reg, int _offset):reg(_reg), offset(_offset){}
    void print(std::ostream& os) const override;
};

class AsmStmt{
public:
    virtual void print(std::ostream& os) const = 0;
    virtual ~AsmStmt() = default;
};


class ComputeAsmStmt : public AsmStmt{
public:
    std::shared_ptr<AsmOpd> lhs;
    std::shared_ptr<AsmOpd> rhs1;
    Operator op;
    dataType type;
    std::shared_ptr<AsmOpd> rhs2;

    ComputeAsmStmt(std::shared_ptr<AsmOpd> _lhs, std::shared_ptr<AsmOpd> _rhs1, Operator _op, std::shared_ptr<AsmOpd> _rhs2): lhs(_lhs), rhs1(_rhs1), op(_op), rhs2(_rhs2) {
        if(lhs)
            type = std::dynamic_pointer_cast<RegisterAsmOpd>(lhs)->reg->t;
        else
            type = dataType::Float;
    }

    void print(std::ostream& os) const override;
};

class SyscallAsmStmt : public AsmStmt{
public:

    SyscallAsmStmt(){}

    void print(std::ostream& os) const override;
};

class MoveAsmStmt : public AsmStmt{
public:
    enum class MoveType {sw, lw, li, la, liD, sd, ld, move, moveD, movt, movf};

    MoveType type;
    std::shared_ptr<AsmOpd> lhs;
    std::shared_ptr<AsmOpd> rhs1;
    std::shared_ptr<AsmOpd> rhs2;

    MoveAsmStmt(MoveType mt, std::shared_ptr<AsmOpd> lhs, std::shared_ptr<AsmOpd> rhs1, std::shared_ptr<AsmOpd> rhs2=nullptr);

    void print(std::ostream& os) const override;
};

class LabelAsmStmt : public AsmStmt{
public:
    std::shared_ptr<LabelAsmOpd> label;
    LabelAsmStmt(std::shared_ptr<AsmOpd> _label): label(std::dynamic_pointer_cast<LabelAsmOpd>(_label)) {}
    void print(std::ostream& os) const override;
};

class ControlFlowAsmStmt : public AsmStmt{
public:
    void print(std::ostream& os) const override;
};

class GotoAsmStmt : public ControlFlowAsmStmt{
public:

    std::shared_ptr<LabelAsmOpd> label;
    GotoAsmStmt(std::shared_ptr<AsmOpd> _label): label(std::dynamic_pointer_cast<LabelAsmOpd>(_label)) {}
    void print(std::ostream& os) const override;
};

class IfGotoAsmStmt : public ControlFlowAsmStmt{
public:
    std::shared_ptr<LabelAsmOpd> label;
    std::shared_ptr<AsmOpd> var;
    IfGotoAsmStmt(std::shared_ptr<AsmOpd> _label, std::shared_ptr<AsmOpd> _var): label(std::dynamic_pointer_cast<LabelAsmOpd>(_label)), var(_var) {}

    void print(std::ostream& os) const override;
};

class JumpRegAsmStmt : public ControlFlowAsmStmt{
public:
    std::shared_ptr<RegisterAsmOpd> reg;
    JumpRegAsmStmt(){
        reg = std::make_shared<RegisterAsmOpd>(std::make_shared<SpecialRegister>(specialRegType::ra));
    }
    JumpRegAsmStmt(std::shared_ptr<RegisterAsmOpd> _reg):reg(_reg){}
    void print(std::ostream& os) const override;
};

class CallAsmStmt : public ControlFlowAsmStmt{
public:
    std::shared_ptr<AsmOpd> func;
    CallAsmStmt(std::shared_ptr<AsmOpd> _func):func(_func){}
    void print(std::ostream& os) const override;
};

} // namespace Sclp
#endif