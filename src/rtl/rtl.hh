#ifndef RTL_HH
#define RTL_HH

#include <string>
#include <memory>
#include <variant>
#include <ostream>
#include <map>
#include "operator.hh"
#include "rtl_registers.hh"
#include "rtl_visitor.hh"
#include "asm_annotation.hh"

namespace Sclp{

class RtlOpd{
public:
    AsmOpdAnnotation asm_an;
    
    virtual void print(std::ostream& os) const = 0;
    virtual ~RtlOpd() = default;

    virtual void accept(RtlVisitor& v) = 0;
};

class ConstRtlOpd : public RtlOpd{
    
};

class DoubleConstRtlOpd : public ConstRtlOpd{
public:
    double value;
    DoubleConstRtlOpd(double _value):value(_value){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class IntConstRtlOpd : public ConstRtlOpd{
public:
    int value;
    IntConstRtlOpd(int _value):value(_value){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class StrConstRtlOpd : public ConstRtlOpd{
public:
    int str_num;
    StrConstRtlOpd(int _str_num): str_num(_str_num){}
    
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class LabelRtlOpd : public RtlOpd{
public:
    int label;
    LabelRtlOpd(int _label):label(_label){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class VariableRtlOpd : public RtlOpd{
public:
    std::string name;
    VariableRtlOpd(std::string _name):name(_name){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class RegisterRtlOpd : public RtlOpd{
public:
    std::shared_ptr<Register> reg;
    RegisterRtlOpd(std::shared_ptr<Register> _reg):reg(_reg){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class RtlStmt{
public:
    AsmStmtAnnotation asm_an;

    virtual void print(std::ostream& os) const = 0;
    virtual ~RtlStmt() = default;

    virtual void accept(RtlVisitor& v) = 0;
};


class ComputeRtlStmt : public RtlStmt{
public:
    std::shared_ptr<RtlOpd> lhs;
    std::shared_ptr<RtlOpd> rhs1;
    Operator op;
    dataType type;
    std::shared_ptr<RtlOpd> rhs2;

    ComputeRtlStmt(std::shared_ptr<RtlOpd> _lhs, std::shared_ptr<RtlOpd> _rhs1, Operator _op, std::shared_ptr<RtlOpd> _rhs2): lhs(_lhs), rhs1(_rhs1), op(_op), rhs2(_rhs2) {
        if(lhs)
            type = std::dynamic_pointer_cast<RegisterRtlOpd>(lhs)->reg->t;
        else
            type = dataType::Float;
    }

    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class WriteRtlStmt : public RtlStmt{
public:

    WriteRtlStmt(){}

    void print(std::ostream& os) const override;

    static std::shared_ptr<IntConstRtlOpd> getSyscallNo(dataType t){
        static std::map<dataType,int> mp = {
            {dataType::Int,1},
            {dataType::Float,3},
            {dataType::String,4}
        };
        return std::make_shared<IntConstRtlOpd>(mp.at(t));
    }

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class ReadRtlStmt : public RtlStmt{
public:

    ReadRtlStmt(){}

    void print(std::ostream& os) const override;

    static std::shared_ptr<IntConstRtlOpd> getSyscallNo(dataType t){
        static std::map<dataType,int> mp = {
            {dataType::Int,5},
            {dataType::Float,7},
        };
        return std::make_shared<IntConstRtlOpd>(mp.at(t));
    }

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class MoveRtlStmt : public RtlStmt{
public:
    enum class MoveType {iLoad,store,load,load_addr,iLoadD,storeD,loadD,move,moveD,movt,movf};

    MoveType type;
    std::shared_ptr<RtlOpd> lhs;
    std::shared_ptr<RtlOpd> rhs1;
    std::shared_ptr<RtlOpd> rhs2; // for movt, movf

    MoveRtlStmt(std::shared_ptr<RtlOpd> _lhs = nullptr, std::shared_ptr<RtlOpd> _rhs1 = nullptr, std::shared_ptr<RtlOpd> _rhs2 = nullptr, std::optional<MoveType> _type = std::nullopt);

    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class StackOpRtlStmt : public RtlStmt{
public:
    enum class OpType {push,pop};

    OpType type;
    std::shared_ptr<RtlOpd> lhs;
    dataType dType;

    StackOpRtlStmt(std::shared_ptr<RtlOpd> _lhs, dataType _dType, std::optional<OpType> _type):type(*_type), lhs(_lhs), dType(_dType) {}

    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class LabelRtlStmt : public RtlStmt{
public:
    std::shared_ptr<LabelRtlOpd> label;
    LabelRtlStmt(std::shared_ptr<RtlOpd> _label): label(std::dynamic_pointer_cast<LabelRtlOpd>(_label)) {}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class ControlFlowRtlStmt : public RtlStmt{
public:
    void print(std::ostream& os) const override;
};

class GotoRtlStmt : public ControlFlowRtlStmt{
public:

    std::shared_ptr<LabelRtlOpd> label;
    GotoRtlStmt(std::shared_ptr<RtlOpd> _label): label(std::dynamic_pointer_cast<LabelRtlOpd>(_label)) {}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class IfGotoRtlStmt : public ControlFlowRtlStmt{
public:
    std::shared_ptr<LabelRtlOpd> label;
    std::shared_ptr<RtlOpd> var;
    IfGotoRtlStmt(std::shared_ptr<RtlOpd> _label, std::shared_ptr<RtlOpd> _var): label(std::dynamic_pointer_cast<LabelRtlOpd>(_label)), var(_var) {}

    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class ReturnRtlStmt : public ControlFlowRtlStmt{
public:
    std::shared_ptr<RtlOpd> var;
    ReturnRtlStmt(std::shared_ptr<RtlOpd> _var):var(_var){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

class CallRtlStmt : public ControlFlowRtlStmt{
public:
    std::optional<std::shared_ptr<RtlOpd>> lhs;
    std::shared_ptr<RtlOpd> func;
    CallRtlStmt(std::optional<std::shared_ptr<RtlOpd>> _lhs,std::shared_ptr<RtlOpd> _func):lhs(_lhs),func(_func){}
    void print(std::ostream& os) const override;

    void accept(RtlVisitor& v) override{
        v.visit(*this);
    }
};

} // namespace Sclp
#endif