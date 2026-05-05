#ifndef TAC_HH
#define TAC_HH

#include <string>
#include <memory>
#include <variant>
#include <ostream>
#include "operator.hh"
#include "tac_visitor.hh"
#include "rtl_annotation.hh"

namespace Sclp{

class TacOpd{
public:
    RtlOpdAnnotation rtl_an;
    virtual void print(std::ostream& os) const = 0;
    virtual ~TacOpd() = default;

    dataType type;

    virtual bool is_temp() {return false;}
    virtual bool is_const() {return false;}
    virtual bool is_var() {return false;}

    virtual void accept(TacVisitor& v) = 0;
};

class ConstTacOpd : public TacOpd{
    bool is_const() override {return true;}
};

class DoubleConstTacOpd : public ConstTacOpd{
public:
    double value;
    DoubleConstTacOpd(double _value):value(_value){type = dataType::Float;}
    void print(std::ostream& os) const override;

    void accept(TacVisitor& v) override{
        v.visit(*this);
    }

};

class IntConstTacOpd : public ConstTacOpd{
public:
    int value;
    IntConstTacOpd(int _value):value(_value){type = dataType::Int;}
    void print(std::ostream& os) const override;

    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class StrConstTacOpd : public ConstTacOpd{
public:
    std::string value;
    StrConstTacOpd(std::string _value):value(_value){type = dataType::String;}
    void print(std::ostream& os) const override;

    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class LabelTacOpd : public TacOpd{
    static size_t nextLabel;
public:
    size_t label;
    LabelTacOpd():label(nextLabel++){}
    static void reset(){
        nextLabel = 0;
    }
    void print(std::ostream& os) const override;

    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class VarTempTacOpd : public TacOpd {
};

class TempraryTacOpd : public VarTempTacOpd{
    static size_t nextTemp;
    static size_t nextSTemp;
public:
    bool isSTemp;
    size_t place;

    bool is_temp() override {return !isSTemp;}
    bool is_var() override {return isSTemp;}

    static void reset(){
        nextTemp = nextSTemp = 0;
    }
    TempraryTacOpd (dataType _type, bool _isSTemp = false):isSTemp(_isSTemp){
        place = isSTemp ? nextSTemp++ : nextTemp++;
        type = _type;
    }
    void print(std::ostream& os) const override;

    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class VariableTacOpd : public VarTempTacOpd{
public:
    std::string name;
    bool is_var() override {return true;}
    
    VariableTacOpd(std::string _name, dataType _type):name(_name){type = _type;}
    void print(std::ostream& os) const override;

    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};


class TacStmt{
public:
    RtlStmtAnnotation rtl_an;
    virtual void print(std::ostream& os) const = 0;
    virtual ~TacStmt() = default;
    virtual void accept(TacVisitor& v) = 0;
};

class AsgnTacStmt : public TacStmt{
public:
    std::shared_ptr<TacOpd> lhs;
    std::shared_ptr<TacOpd> rhs;

    AsgnTacStmt(std::shared_ptr<TacOpd> _lhs, std::shared_ptr<TacOpd> _rhs):lhs(_lhs),rhs(_rhs){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }

};

class ComputeTacStmt : public TacStmt{
public:
    std::shared_ptr<TacOpd> lhs;
    std::shared_ptr<TacOpd> rhs1;
    Operator type;
    std::shared_ptr<TacOpd> rhs2;

    ComputeTacStmt(std::shared_ptr<VarTempTacOpd> _lhs, std::shared_ptr<TacOpd> _rhs1, Operator _type,std::shared_ptr<TacOpd> _rhs2):lhs(_lhs),rhs1(_rhs1),type(_type),rhs2(_rhs2){}

    ComputeTacStmt(std::shared_ptr<TacOpd> _lhs, Operator _type,std::shared_ptr<TacOpd> _rhs1):lhs(_lhs),rhs1(_rhs1),type(_type){}

    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class IOTacStmt : public TacStmt{
public:
    enum class Op {READ, WRITE};
    Op op;
    std::shared_ptr<TacOpd> var;
    IOTacStmt(Op _op, std::shared_ptr<TacOpd> _var):op(_op), var(_var){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class GotoTacStmt : public TacStmt{
public:
    std::shared_ptr<LabelTacOpd> label;
    GotoTacStmt(std::shared_ptr<LabelTacOpd> _label):label(_label){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class LabelTacStmt : public TacStmt{
public:
    std::shared_ptr<LabelTacOpd> label;
    LabelTacStmt(std::shared_ptr<LabelTacOpd> _label):label(_label){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class IfGotoTacStmt : public TacStmt{
public:
    std::shared_ptr<TacOpd> var;
    std::shared_ptr<LabelTacOpd> label;
    IfGotoTacStmt(std::shared_ptr<TacOpd> _var, std::shared_ptr<LabelTacOpd> _label):var(_var), label(_label){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class ReturnTacStmt : public TacStmt{
public:
    std::shared_ptr<TacOpd> var;
    ReturnTacStmt(std::shared_ptr<TacOpd> _var):var(_var){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

class CallTacStmt : public TacStmt{
public:
    std::optional<std::shared_ptr<TacOpd>> lhs;
    std::shared_ptr<VariableTacOpd> func;
    std::vector<std::shared_ptr<TacOpd>> args;
    CallTacStmt(std::optional<std::shared_ptr<TacOpd>> _lhs,std::shared_ptr<VariableTacOpd> _func, std::vector<std::shared_ptr<TacOpd>> _args):lhs(_lhs),func(_func),args(_args){}
    void print(std::ostream& os) const override;
    void accept(TacVisitor& v) override{
        v.visit(*this);
    }
};

} // namespace Sclp
#endif