#ifndef ENUM_HH
#define ENUM_HH
#include<variant>
#include<map>
#include<string>

namespace Sclp{
enum class AbOp {ADD, SUB, MULT, DIV};
enum class AuOp {UMINUS};
enum class BbOp {AND, OR};
enum class BuOp {NOT};
enum class RbOp {GT, LT, GTE, LTE, EQ, NEQ};

using Operator = std::variant<RbOp,AuOp,AbOp,BuOp,BbOp>;

const std::map<Operator,std::string> opToString {
    {BbOp::AND, "&&"},
    {BbOp::OR, "||"},
    {AbOp::ADD, "+"},
    {AbOp::SUB, "-"},
    {AbOp::MULT, "*"},
    {AbOp::DIV, "/"},
    {RbOp::GT, ">"},
    {RbOp::LT, "<"},
    {RbOp::GTE, ">="},
    {RbOp::LTE, "<="},
    {RbOp::EQ, "=="},
    {RbOp::NEQ, "!="},
    {BuOp::NOT, "!"},
    {AuOp::UMINUS, "-"},
};

const std::map<Operator,std::string> opToRtl {
    {BbOp::AND, "and"},
    {BbOp::OR, "or"},
    {AbOp::ADD, "add"},
    {AbOp::SUB, "sub"},
    {AbOp::MULT, "mul"},
    {AbOp::DIV, "div"},
    {RbOp::GT, "sgt"},
    {RbOp::LT, "slt"},
    {RbOp::GTE, "sge"},
    {RbOp::LTE, "sle"},
    {RbOp::EQ, "seq"},
    {RbOp::NEQ, "sne"},
    {BuOp::NOT, "not"},
    {AuOp::UMINUS, "uminus"},
};

const std::map<Operator,std::string> opToAsm {
    {BbOp::AND, "and"},
    {BbOp::OR, "or"},
    {AbOp::ADD, "add"},
    {AbOp::SUB, "sub"},
    {AbOp::MULT, "mul"},
    {AbOp::DIV, "div"},
    {RbOp::GT, "sgt"},
    {RbOp::LT, "slt"},
    {RbOp::GTE, "sge"},
    {RbOp::LTE, "sle"},
    {RbOp::EQ, "seq"},
    {RbOp::NEQ, "sne"},
    // {BuOp::NOT, "not"},
    {AuOp::UMINUS, "neg"},
};

const std::map<Operator,std::string> opToAsmFloat {
    {BbOp::AND, "and.d"},
    {BbOp::OR, "or.d"},
    {AbOp::ADD, "add.d"},
    {AbOp::SUB, "sub.d"},
    {AbOp::MULT, "mul.d"},
    {AbOp::DIV, "div.d"},
    {RbOp::LT, "c.lt.d"},
    {RbOp::LTE, "c.le.d"},
    {RbOp::EQ, "c.eq.d"},
    {AuOp::UMINUS, "neg.d"},
};

}
#endif