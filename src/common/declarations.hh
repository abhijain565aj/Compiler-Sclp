#ifndef DECL_HH
#define DECL_HH
namespace Sclp{
    //program.hh
    class Program;
    class SymbolEntry;
    class SymbolTable;
    class FunctionSymbolEntry;
    class FunctionSymbolTable;
    class Procedure;

    //ast.hh
    enum class dataType {Int, Float, Bool, String, Void};

    class Ast;
    class StatementAst;
    class ExpressionAst;

    class BaseExprAst;
    class UnaryExprAst;
    class BinaryExprAst;
    class TernaryExprAst;

    class FunctionCallAst;
    class NameExprAst;
    template<typename T> class NumberExprAst;
    class StringExprAst;

    class ArithmeticUnaryExprAst;
    class BooleanUnaryExprAst;
    class ArithmeticBinaryExprAst;
    class BooleanBinaryExprAst;
    class RelationalBinaryExprAst;
    class ConditionalExprAst;

    class AssignmentStmtAst;
    class ReadStmtAst;
    class WriteStmtAst;
    class IterationStmtAst;
    class SelectionStmtAst;
    class SequenceStmtAst;
    class ReturnStmtAst;
    class CallStmtAst;

    // tac.hh
     class TacOpd;
     class ConstTacOpd;
     class DoubleConstTacOpd;
     class IntConstTacOpd;
     class StrConstTacOpd;
     class LabelTacOpd;
     class VarTempTacOpd;
     class TempraryTacOpd;
     class VariableTacOpd;

     class TacStmt;
     class AsgnTacStmt;
     class ComputeTacStmt;
     class IOTacStmt;
     class GotoTacStmt;
     class LabelTacStmt;
     class IfGotoTacStmt;
     class CallTacStmt;
     class ReturnTacStmt;

    // rtl_builder.hh
    class RtlBuilder;

    //rtl.hh
    class RtlOpd;
    class ConstRtlOpd;
    class DoubleConstRtlOpd;
    class IntConstRtlOpd;
    class StrConstRtlOpd;
    class LabelRtlOpd;
    class VariableRtlOpd;
    class RegisterRtlOpd;

    class RtlStmt;
    class ComputeRtlStmt;
    class WriteRtlStmt;
    class ReadRtlStmt;
    class MoveRtlStmt;
    class StackOpRtlStmt;
    class LabelRtlStmt;
    class ControlFlowRtlStmt;
    class GotoRtlStmt;
    class IfGotoRtlStmt;
    class ReturnRtlStmt;
    class CallRtlStmt;
} //namespace Sclp
#endif