#ifndef AST_HH
#define AST_HH

#include <memory>
#include <string>
#include <vector>

#include "location.hh"
#include "declarations.hh"
#include "tac_annotation.hh"
#include "ast_visitor.hh"
#include "operator.hh"

namespace Sclp{

class Ast{
public:
    location loc;
    TacAnnotation tac_an;

    Ast() = default;
    explicit Ast(const location &l): loc(l) {}

    virtual ~Ast() = default;

    virtual void accept(AstVisitor &v) = 0;
};

class StatementAst: public Ast{
public:
    using Ptr = std::unique_ptr<StatementAst>;

    StatementAst() = default;
    explicit StatementAst(const location &l): Ast(l) {}

    virtual ~StatementAst() = default;

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class ExpressionAst: public Ast{
protected:
    dataType type = dataType::Void;

public:
    using Ptr = std::unique_ptr<ExpressionAst>;

    ExpressionAst() = default;
    explicit ExpressionAst(const location &l): Ast(l) {}

    virtual ~ExpressionAst() = default;

    dataType getType(){
        return type;
    }

    void setType(dataType t){
        this->type = t;
    }

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

/* EXPRESSION ASTS */

class BaseExprAst: public ExpressionAst{
public:
    using ExpressionAst::ExpressionAst;

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class UnaryExprAst: public ExpressionAst{
public:
    enum class OpType {Arithmetic, Boolean};

    OpType op;
    ExpressionAst::Ptr operand;

    UnaryExprAst(OpType _op, ExpressionAst::Ptr expr, const location &l): ExpressionAst(l), op(_op), operand(std::move(expr)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class BinaryExprAst: public ExpressionAst{
public:
    enum class OpType {Arithmetic, Boolean, Relational};

    OpType opType;
    ExpressionAst::Ptr lhs, rhs;

    BinaryExprAst(OpType o, ExpressionAst::Ptr _lhs, ExpressionAst::Ptr _rhs, const location &l): ExpressionAst(l), opType(o), lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class TernaryExprAst: public ExpressionAst{
public:
    enum class OpType {CondExpr};

    OpType opType;
    ExpressionAst::Ptr expr1, expr2, expr3;

    TernaryExprAst(OpType o, ExpressionAst::Ptr _expr1, ExpressionAst::Ptr _expr2, ExpressionAst::Ptr _expr3, const location &l): ExpressionAst(l), opType(o), expr1(std::move(_expr1)), expr2(std::move(_expr2)), expr3(std::move(_expr3)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

/* Base Expr */

class FunctionCallAst : public  BaseExprAst{
public:
    std::string name;
    std::vector<ExpressionAst::Ptr> args;
    
    FunctionCallAst(std::string _name, std::vector<ExpressionAst::Ptr> _args, const location&l):BaseExprAst(l),name(std::move(_name)),args(std::move(_args)){}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class NameExprAst: public BaseExprAst{
public:
    std::string name;

    NameExprAst(std::string n, const location &l) : BaseExprAst(l), name(std::move(n)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

template<typename T>
class NumberExprAst: public BaseExprAst{
public:
    T value;

    NumberExprAst(T v, const location &l) : BaseExprAst(l), value(v) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class StringExprAst: public BaseExprAst{
public:
    std::string value;

    StringExprAst(std::string v, const location &l): BaseExprAst(l), value(std::move(v)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

/* Unary Expr */

class ArithmeticUnaryExprAst: public UnaryExprAst{
public:    
    AuOp op;

    ArithmeticUnaryExprAst(ExpressionAst::Ptr _operand, AuOp _op, const location &l): UnaryExprAst(OpType::Arithmetic, std::move(_operand), l), op(_op){}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class BooleanUnaryExprAst: public UnaryExprAst{
public:    
    BuOp op;

    BooleanUnaryExprAst(ExpressionAst::Ptr _operand, BuOp _op, const location &l): UnaryExprAst(OpType::Boolean, std::move(_operand), l), op(_op){}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

/* Binary Expr */

class ArithmeticBinaryExprAst: public BinaryExprAst{
public:
    AbOp op;
    ArithmeticBinaryExprAst(ExpressionAst::Ptr _lhs, ExpressionAst::Ptr _rhs, AbOp o, const location &l): BinaryExprAst(OpType::Arithmetic, std::move(_lhs), std::move(_rhs), l), op(o) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class BooleanBinaryExprAst: public BinaryExprAst{
public:
    BbOp op;
    BooleanBinaryExprAst(ExpressionAst::Ptr _lhs, ExpressionAst::Ptr _rhs, BbOp o, const location &l): BinaryExprAst(OpType::Boolean, std::move(_lhs), std::move(_rhs), l), op(o) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class RelationalBinaryExprAst: public BinaryExprAst{
public:
    RbOp op;
    
    RelationalBinaryExprAst(ExpressionAst::Ptr _lhs, ExpressionAst::Ptr _rhs, RbOp o, const location &l): BinaryExprAst(OpType::Relational, std::move(_lhs), std::move(_rhs), l), op(o) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

/* Ternary Exps */

class ConditionalExprAst: public TernaryExprAst{
public:
    ConditionalExprAst(ExpressionAst::Ptr _exp1, ExpressionAst::Ptr _exp2, ExpressionAst::Ptr _exp3, const location &l): TernaryExprAst(OpType::CondExpr, std::move(_exp1), std::move(_exp2), std::move(_exp3), l) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

/* STATEMENT ASTS */

class AssignmentStmtAst: public StatementAst{
public:
    ExpressionAst::Ptr lhs;
    ExpressionAst::Ptr rhs;

    AssignmentStmtAst(ExpressionAst::Ptr _lhs, ExpressionAst::Ptr _rhs, const location &l): StatementAst(l), lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class ReadStmtAst: public StatementAst{
public:
    ExpressionAst::Ptr target;

    ReadStmtAst(ExpressionAst::Ptr _target, const location &l): StatementAst(l), target(std::move(_target)) {}  

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class WriteStmtAst: public StatementAst{
public:
    ExpressionAst::Ptr target;

    WriteStmtAst(ExpressionAst::Ptr _target, const location &l): StatementAst(l), target(std::move(_target)) {}  

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class IterationStmtAst: public StatementAst{
public:
    enum class Type {WHILE, DO_WHILE};
    Type type;
    ExpressionAst::Ptr cond;
    StatementAst::Ptr stmt;
    IterationStmtAst(Type _type, ExpressionAst::Ptr _cond, StatementAst::Ptr _stmt, location &l): StatementAst(l), type(_type), cond(std::move(_cond)), stmt(std::move(_stmt)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class SelectionStmtAst: public StatementAst{
public:
    enum class Type {ONLY_IF, IF_ELSE};
    Type type;
    ExpressionAst::Ptr cond;
    StatementAst::Ptr stmt1;
    StatementAst::Ptr stmt2;
    SelectionStmtAst(ExpressionAst::Ptr _cond, StatementAst::Ptr _stmt1, const location &l): 
        StatementAst(l), type(Type::ONLY_IF), cond(std::move(_cond)), stmt1(std::move(_stmt1)) {}
    SelectionStmtAst(ExpressionAst::Ptr _cond, StatementAst::Ptr _stmt1, StatementAst::Ptr _stmt2, const location &l): 
        StatementAst(l), type(Type::IF_ELSE), cond(std::move(_cond)), stmt1(std::move(_stmt1)), stmt2(std::move(_stmt2)) {}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class SequenceStmtAst: public StatementAst{
public:
    std::vector<StatementAst::Ptr> stmts;
    SequenceStmtAst(std::vector<StatementAst::Ptr> _stmts, const location &l): 
        StatementAst(l), stmts(std::move(_stmts)){}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class ReturnStmtAst: public StatementAst{
public:
    ExpressionAst::Ptr exp;
    ReturnStmtAst(ExpressionAst::Ptr _exp, const location&l):
        StatementAst(l), exp(std::move(_exp)){}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

class CallStmtAst: public StatementAst{
public:
    ExpressionAst::Ptr exp;
    CallStmtAst(ExpressionAst::Ptr _exp, const location&l):
        StatementAst(l), exp(std::move(_exp)){}

    void accept(AstVisitor &v) override{
        v.visit(*this);
    }
};

}

#endif