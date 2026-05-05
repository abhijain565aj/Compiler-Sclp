#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include "ast_printer.hh"

template<typename F, typename... Args>
void Sclp::AstPrinter::indent(int x, F&& f, Args&&... args){
    indent_level += x;
    for (int i=0; i<indent_level; i++) ast_file_stream <<" ";
    f(std::forward<Args>(args)...);
    indent_level-=x;
}

template<typename F, typename... Args>
void Sclp::AstPrinter::indent_stmt(int x, F&& f, Args&&... args){
    indent_level += x;
    f(std::forward<Args>(args)...);
    indent_level-=x;
}

Sclp::AstPrinter::AstPrinter(std::shared_ptr<Program> p, const std::string& filename): indent_level(0) {
    ast_file_stream = std::ofstream(filename);
    if (!ast_file_stream.is_open()){
        std::cerr<<"Error: Cannot open file "<<filename<<std::endl;
        exit(1);
    }

    auto printTable = p->getSortedFuncDeclarations();
    for (auto &entry : printTable){
        ast_file_stream << "**PROCEDURE: "<< entry->getName() << std::endl;
        indent(6,[&]{
            ast_file_stream << "Return Type: " << TYPE_NAME.at(entry->getReturnType()) << std::endl;
        });
        indent(6,[&]{
            ast_file_stream << "Formal Parameters:" << std::endl;
            auto& paramTable = entry->getParamList()->table;

            for(auto param:paramTable){
                indent(2,[&]{
                    ast_file_stream << param->name<<"  Type:"<<TYPE_NAME.at(param->dType) << std::endl;
                });
            }
        });
        ast_file_stream << "**BEGIN: Abstract Syntax Tree ";
        entry->procedure->accept(*this);
        ast_file_stream << std::endl << "**END: Abstract Syntax Tree " << std::endl;
    }
}

void Sclp::AstPrinter::visit(Procedure& p){
    for (auto &stmt: p.body){
        indent_stmt(9,[&](){
            stmt->accept(*this);
        });
    }
}

void Sclp::AstPrinter::visit(Ast&){
}

void Sclp::AstPrinter::visit(StatementAst&){
}

void Sclp::AstPrinter::visit(ExpressionAst&){

}

void Sclp::AstPrinter::visit(BaseExprAst&){
}

void Sclp::AstPrinter::visit(UnaryExprAst&){

}

void Sclp::AstPrinter::visit(BinaryExprAst&){
}

void Sclp::AstPrinter::visit(TernaryExprAst&){
}

void Sclp::AstPrinter::visit(FunctionCallAst& exp){
    ast_file_stream<<std::endl;
    indent(0,[&]{
        ast_file_stream<<"FN CALL: "<<exp.name<<"(";
        for(auto& arg:exp.args){
            ast_file_stream<<std::endl;
            indent(2,[&]{
                arg->accept(*this);
            });
        }
        ast_file_stream<<")";
    });
}

void Sclp::AstPrinter::visit(NameExprAst& exp){
    ast_file_stream << "Name : "<<exp.name<<TYPE_NAME.at(exp.getType());
}

void Sclp::AstPrinter::visit(NumberExprAst<int>& exp){
    ast_file_stream << "Num : "<<exp.value<<TYPE_NAME.at(exp.getType());
}

void Sclp::AstPrinter::visit(NumberExprAst<double>& exp){
    ast_file_stream << std::fixed << std::setprecision(2) << "Num : "<<exp.value<<TYPE_NAME.at(exp.getType());
}

void Sclp::AstPrinter::visit(StringExprAst& exp){
    ast_file_stream << "String : "<<exp.value<<TYPE_NAME.at(exp.getType());
}

void Sclp::AstPrinter::visit(ArithmeticUnaryExprAst& exp){
    static const std::map<AuOp,std::string> mp = {
        {AuOp::UMINUS,"Uminus"},
    };
    ast_file_stream<<std::endl;
    indent(2,[&]{
        ast_file_stream<<"Arith: "<<mp.at(exp.op)<<TYPE_NAME.at(exp.getType())<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"L_Opd (";
            exp.operand->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(BooleanUnaryExprAst& exp){
    static const std::map<BuOp,std::string> mp = {
        {BuOp::NOT,"NOT"},
    };
    ast_file_stream<<std::endl;
    indent(2,[&]{
        ast_file_stream<<"Condition: "<<mp.at(exp.op)<<TYPE_NAME.at(exp.getType())<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"L_Opd (";
            exp.operand->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(ArithmeticBinaryExprAst& exp){
    static const std::map<AbOp,std::string> mp = {
        {AbOp::ADD,"Plus"},
        {AbOp::SUB,"Minus"},
        {AbOp::MULT,"Mult"},
        {AbOp::DIV,"Div"}
    };
    ast_file_stream<<std::endl;
    indent(2,[&]{
        ast_file_stream<<"Arith: "<<mp.at(exp.op)<<TYPE_NAME.at(exp.getType())<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"L_Opd (";
            exp.lhs->accept(*this);
            ast_file_stream<<")"<<std::endl;
        });
        indent(2,[&]{
            ast_file_stream<<"R_Opd (";
            exp.rhs->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(BooleanBinaryExprAst& exp){
    static const std::map<BbOp,std::string> mp = {
        {BbOp::AND,"AND"},
        {BbOp::OR,"OR"}
    };
    ast_file_stream<<std::endl;
    indent(0,[&]{
        ast_file_stream <<"Condition: "<<mp.at(exp.op)<<TYPE_NAME.at(exp.getType())<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"L_Opd (";
            exp.lhs->accept(*this);
            ast_file_stream<<")"<<std::endl;
        });
        indent(2,[&]{
            ast_file_stream<<"R_Opd (";
            exp.rhs->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(RelationalBinaryExprAst& exp){
    static const std::map<RbOp,std::string> mp = {
        {RbOp::GT,"GT"},
        {RbOp::GTE,"GE"},
        {RbOp::LT,"LT"},
        {RbOp::LTE,"LE"},
        {RbOp::EQ,"EQ"},
        {RbOp::NEQ,"NE"},
    };
    ast_file_stream<<std::endl;
    indent(2,[&]{
        ast_file_stream<<"Condition: "<<mp.at(exp.op)<<TYPE_NAME.at(exp.getType())<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"L_Opd (";
            exp.lhs->accept(*this);
            ast_file_stream<<")"<<std::endl;
        });
        indent(2,[&]{
            ast_file_stream<<"R_Opd (";
            exp.rhs->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(ConditionalExprAst& exp){
    exp.expr1->accept(*this);
    ast_file_stream<<std::endl;
    indent(2,[&]{
        indent(0,[&]{
            ast_file_stream<<"True_Part (";
            exp.expr2->accept(*this);
            ast_file_stream<<")"<<std::endl;
        });
        indent(0,[&]{
            ast_file_stream<<"False_Part (";
            exp.expr3->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(AssignmentStmtAst& exp){
    ast_file_stream<<std::endl;
    indent(0,[&]{
        ast_file_stream<<"Asgn:"<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"LHS (";
            exp.lhs->accept(*this);
            ast_file_stream<<")"<<std::endl;
        });
        indent(2,[&]{
            ast_file_stream<<"RHS (";
            exp.rhs->accept(*this);
            ast_file_stream<<")";
        });
    });
}

void Sclp::AstPrinter::visit(ReadStmtAst& exp){
    ast_file_stream<<std::endl;
    indent(0,[&]{
        ast_file_stream <<"Read: ";
        exp.target->accept(*this);
    });
}

void Sclp::AstPrinter::visit(WriteStmtAst& exp){
    ast_file_stream<<std::endl;
    indent(0,[&]{
        ast_file_stream <<"Write: ";
        exp.target->accept(*this);
    });
}

void Sclp::AstPrinter::visit(IterationStmtAst& stmt){
    ast_file_stream<<std::endl;
    indent(0,[&]{
        if(stmt.type == IterationStmtAst::Type::WHILE){
            ast_file_stream<<"While: "<<std::endl;
            indent(2,[&]{
                ast_file_stream<<"Condition (";
                stmt.cond->accept(*this);
                ast_file_stream<<")"<<std::endl;
            });
            indent(2,[&]{
                ast_file_stream<<"Body (";
                indent_stmt(2,[&]{stmt.stmt->accept(*this);});
                ast_file_stream<<")";
            }); 
        }
        else if(stmt.type == IterationStmtAst::Type::DO_WHILE){
            ast_file_stream<<"Do: "<<std::endl;
            indent(2,[&]{
                ast_file_stream<<"Body (";
                indent_stmt(2,[&]{stmt.stmt->accept(*this);});
                ast_file_stream<<")"<<std::endl;
            }); 
            indent(2,[&]{
                ast_file_stream<<"While Condition (";
                stmt.cond->accept(*this);
                ast_file_stream<<")";
            });
        }
    });
}


void Sclp::AstPrinter::visit(SelectionStmtAst& stmt){
    ast_file_stream<<std::endl;
    indent(0,[&]{
        ast_file_stream<<"If: "<<std::endl;
        indent(2,[&]{
            ast_file_stream<<"Condition (";
            stmt.cond->accept(*this);
            ast_file_stream<<")"<<std::endl;
        });
        indent(2,[&]{
            ast_file_stream<<"Then (";
            indent_stmt(2,[&]{stmt.stmt1->accept(*this);});
            ast_file_stream<<")";
        });
        if(stmt.type == SelectionStmtAst::Type::IF_ELSE){
            ast_file_stream<<std::endl;
            indent(2,[&]{
                ast_file_stream<<"Else (";
                indent_stmt(2,[&]{stmt.stmt2->accept(*this);});
                ast_file_stream<<")";
            });
        }
    });
}
    
void Sclp::AstPrinter::visit(SequenceStmtAst& stmt){
    for(auto &s:stmt.stmts){
        indent_stmt(0,[&]{
            s->accept(*this);
        });
    }
}

void Sclp::AstPrinter::visit(ReturnStmtAst& stmt){
    ast_file_stream<<std::endl;
    indent_stmt(0,[&]{
        ast_file_stream<<"Return:";
        stmt.exp->accept(*this);
    });
}

void Sclp::AstPrinter::visit(CallStmtAst& stmt){
    stmt.exp->accept(*this);
}