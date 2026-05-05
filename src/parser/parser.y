%require "3.2"
%language "c++"
%define api.token.constructor
%define api.value.type variant

%define api.namespace {Sclp}
%define api.parser.class {Parser}

%locations
// %define api.location.type {location}

%code requires {
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wsign-conversion"

    #include <string>
    #include <vector>
    #include <memory>
    #include <utility>
    #include "ast.hh"
    #include "program.hh"

    namespace Sclp {
        class Driver;
        class Scanner;
    }
}

%code {
    #include "scanner.hh"
    #include "driver.hh"
    
    static Sclp::Parser::symbol_type yylex(Sclp::Scanner& scanner){
        return scanner.lex();
    }
}

%lex-param { Sclp::Scanner& scanner }
%parse-param { Sclp::Scanner& scanner }
%parse-param { Sclp::Driver& driver }

%token INTEGER FLOAT BOOL STRING VOID
%token WRITE READ RETURN
%token IF WHILE DO ELSE
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
%token COMMA SEMICOLON 
%token ASSIGN_OP PLUS MINUS MULT DIV
%token LESS_THAN LESS_THAN_EQUAL GREATER_THAN GREATER_THAN_EQUAL EQUAL NOT_EQUAL
%token QUESTION_MARK COLON 
%token OR AND NOT

%token <std::string> NAME
%token <int> INT_NUM 
%token <double> FLOAT_NUM 
%token <std::string> STR_CONST
%token ERROR

%type <std::unique_ptr<Sclp::ExpressionAst>> exp rel_exp var_as_operand const_as_operand var_name if_condition actual_arg func_call
%type <std::unique_ptr<Sclp::StatementAst>> stmt assignment_stmt read_stmt print_stmt if_stmt do_while_stmt while_stmt compound_stmt call_stmt return_stmt
%type <std::vector<std::unique_ptr<Sclp::StatementAst>>> stmt_list

%type <Sclp::dataType> param_type named_type
%type <std::string> var_delc_item
%type <std::vector<std::string>> var_delc_item_list

%type <std::shared_ptr<SymbolEntry>> formal_param
%type <std::shared_ptr<FunctionSymbolEntry>> func_decl

%type <std::unique_ptr<Sclp::SymbolTable>> formal_param_list optional_local_var_decl_stmt_list var_decl_stmt var_decl_stmt_list
%type <std::pair<std::unique_ptr<Sclp::SymbolTable>, std::unique_ptr<Sclp::FunctionSymbolTable>>> global_decl_stmt_list

%type <std::vector<std::unique_ptr<Sclp::ExpressionAst>>> actual_arg_list non_empty_arg_list
%type <std::vector<std::shared_ptr<Sclp::Procedure>>> func_def_list
%type <std::shared_ptr<Sclp::Procedure>> func_def
%type <std::pair<Sclp::dataType, std::string>> func_header

%type <std::shared_ptr<Program>> program

%define parse.error verbose

%right QUESTION_MARK COLON
%left OR 
%left AND
%right NOT

%nonassoc LESS_THAN_EQUAL LESS_THAN GREATER_THAN_EQUAL GREATER_THAN EQUAL NOT_EQUAL

%right ASSIGN_OP
%left PLUS MINUS
%left MULT DIV
%right UMINUS

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program

%%
program
    : global_decl_stmt_list func_def_list                        {$$ = std::make_shared<Program>(std::move($1.first), std::move($1.second), std::move($2)); driver.program = std::move($$);}
    | func_def_list                                              {$$ = std::make_shared<Program>(std::make_unique<SymbolTable>(), std::make_unique<Sclp::FunctionSymbolTable>(), std::move($1)); driver.program = std::move($$);}
;

global_decl_stmt_list
    : global_decl_stmt_list func_decl                   {$$ = std::move($1); $$.second->insert(std::move($2));}
    | global_decl_stmt_list var_decl_stmt               {$$ = std::move($1); $$.first->merge(std::move($2));}
    | var_decl_stmt                                     {$$ = std::make_pair(std::move($1),std::make_unique<FunctionSymbolTable>());}
    | func_decl                                         {$$ = std::make_pair(std::make_unique<SymbolTable>(), std::make_unique<FunctionSymbolTable>()); $$.second->insert(std::move($1));}
;

func_decl
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON    {$$ = std::make_shared<Sclp::FunctionSymbolEntry>(std::move($1.second), $1.first, std::move($3));}
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON                      {$$ = std::make_shared<Sclp::FunctionSymbolEntry>(std::move($1.second), $1.first, std::make_unique<Sclp::SymbolTable>());}
;

func_def_list
    : func_def_list func_def    {$$ = std::move($1); $$.push_back($2);}
    | func_def                  {$$ = std::vector<std::shared_ptr<Sclp::Procedure>>(); $$.push_back($1);}
;

func_header
    : named_type NAME                                       {$$ = std::make_pair($1, $2+($2=="main"?"":"_"));}
;

func_def
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_local_var_decl_stmt_list stmt_list RIGHT_CURLY_BRACKET                           {$$ = std::make_shared<Sclp::Procedure>($1.first, $1.second, std::move($3), std::move($6), std::move($7));}
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_local_var_decl_stmt_list stmt_list RIGHT_CURLY_BRACKET                                             {$$ = std::make_shared<Sclp::Procedure>($1.first, $1.second, std::make_unique<Sclp::SymbolTable>() , std::move($5), std::move($6));}
;

formal_param_list
    : formal_param_list COMMA formal_param                  {$$ = std::move($1); $$->insert(std::move($3));}
    | formal_param                                          {$$ = std::make_unique<Sclp::SymbolTable>(); $$->insert(std::move($1));}
;

formal_param
    : param_type NAME                                       {$$ = std::make_shared<SymbolEntry>($2+"_", $1, Sclp::SymbolEntry::symbolType::PARAM);}
;

param_type
    : INTEGER                                               {$$ = Sclp::dataType::Int; }
    | FLOAT                                                 {$$ = Sclp::dataType::Float; }
    | BOOL                                                  {$$ = Sclp::dataType::Bool; }
    | STRING                                                {$$ = Sclp::dataType::String;}
;

stmt_list
    : stmt_list stmt                                        {$$ = std::move($1); $$.push_back(std::move($2));}
    | %empty                                                {$$ = std::vector<std::unique_ptr<Sclp::StatementAst>>();}
;

stmt
    : assignment_stmt                                       {$$ = std::move($1);}
    | if_stmt                                               {$$ = std::move($1);}
    | do_while_stmt                                         {$$ = std::move($1);}
    | while_stmt                                            {$$ = std::move($1);}
    | compound_stmt                                         {$$ = std::move($1);}
    | print_stmt                                            {$$ = std::move($1);}
    | read_stmt                                             {$$ = std::move($1);}
    | call_stmt                                             {$$ = std::move($1);}
    | return_stmt                                           {$$ = std::move($1);}
;

call_stmt
    : func_call SEMICOLON   {$$ = std::make_unique<Sclp::CallStmtAst>(std::move($1), @$);}                                
;

func_call
    : NAME LEFT_ROUND_BRACKET actual_arg_list RIGHT_ROUND_BRACKET {$$ = std::make_unique<Sclp::FunctionCallAst>($1+($1=="main"?"":"_"),std::move($3),@$);}
;

actual_arg_list
    : non_empty_arg_list {$$ = std::move($1);}
    | %empty {$$ = std::vector<std::unique_ptr<Sclp::ExpressionAst>>();}
;

non_empty_arg_list
    : non_empty_arg_list COMMA actual_arg {$$ = std::move($1); $$.push_back(std::move($3));}
    | actual_arg {$$ = std::vector<std::unique_ptr<Sclp::ExpressionAst>>(); $$.push_back(std::move($1));}
;

actual_arg
    : exp   {$$ = std::move($1);}
;

return_stmt
    : RETURN exp SEMICOLON {$$ = std::make_unique<Sclp::ReturnStmtAst>(std::move($2), @$);}
;

optional_local_var_decl_stmt_list
    : %empty                                                {$$ = std::make_unique<Sclp::SymbolTable>();}
    | var_decl_stmt_list                                    {$$ = std::move($1);}
;

var_decl_stmt_list
    : var_decl_stmt                                         {$$ = std::move($1);}
    | var_decl_stmt_list var_decl_stmt                      {$$ = std::move($1); $$->merge(std::move($2));}
;

var_decl_stmt
    : named_type var_delc_item_list SEMICOLON               {$$ = std::make_unique<Sclp::SymbolTable>($1, std::move($2), Sclp::SymbolEntry::symbolType::VAR); }
;

var_delc_item_list
    : var_delc_item_list COMMA var_delc_item                {$$ = std::move($1); $$.push_back(std::move($3));}
    | var_delc_item                                         {$$ = std::vector<std::string>({std::move($1)});}
;

var_delc_item
    : NAME                                                  {$$ = std::move($1)+"_";}
;

named_type
    : INTEGER                                               {$$ = Sclp::dataType::Int; }
    | FLOAT                                                 {$$ = Sclp::dataType::Float; }
    | BOOL                                                  {$$ = Sclp::dataType::Bool; }
    | STRING                                                {$$ = Sclp::dataType::String;}
    | VOID                                                  {$$ = Sclp::dataType::Void;}
;

assignment_stmt
    : var_as_operand ASSIGN_OP exp SEMICOLON        {$$ = std::make_unique<Sclp::AssignmentStmtAst>(std::move($1), std::move($3), @$);}
    | var_as_operand ASSIGN_OP func_call SEMICOLON  {$$ = std::make_unique<Sclp::AssignmentStmtAst>(std::move($1), std::move($3), @$);}      
;

print_stmt
    : WRITE exp SEMICOLON                           {$$ = std::make_unique<Sclp::WriteStmtAst>(std::move($2), @$);}
; 

read_stmt
    : READ var_name SEMICOLON                       {$$ = std::make_unique<Sclp::ReadStmtAst>(std::move($2), @$);}
;

if_condition
    : LEFT_ROUND_BRACKET exp RIGHT_ROUND_BRACKET  {$$ = std::move($2);}
;

if_stmt
    : IF if_condition stmt ELSE stmt    {$$ = std::make_unique<Sclp::SelectionStmtAst>(std::move($2),std::move($3),std::move($5),@$);}
    | IF if_condition stmt %prec LOWER_THAN_ELSE {$$ = std::make_unique<Sclp::SelectionStmtAst>(std::move($2),std::move($3),@$);}
;

do_while_stmt
    : DO stmt WHILE LEFT_ROUND_BRACKET exp RIGHT_ROUND_BRACKET SEMICOLON {$$ = std::make_unique<Sclp::IterationStmtAst>(Sclp::IterationStmtAst::Type::DO_WHILE,std::move($5),std::move($2), @$);}
;

while_stmt
    : WHILE LEFT_ROUND_BRACKET exp RIGHT_ROUND_BRACKET stmt {$$ = std::make_unique<Sclp::IterationStmtAst>(Sclp::IterationStmtAst::Type::WHILE,std::move($3),std::move($5), @$);}
;

compound_stmt
    : LEFT_CURLY_BRACKET stmt_list RIGHT_CURLY_BRACKET {$$ = std::make_unique<Sclp::SequenceStmtAst>(std::move($2),@$);}
;

exp
    : exp PLUS exp                                  {$$ = std::make_unique<Sclp::ArithmeticBinaryExprAst>(std::move($1), std::move($3), Sclp::AbOp::ADD, @$);}
    | exp MINUS exp                                 {$$ = std::make_unique<Sclp::ArithmeticBinaryExprAst>(std::move($1), std::move($3), Sclp::AbOp::SUB, @$);}
    | exp MULT exp                                  {$$ = std::make_unique<Sclp::ArithmeticBinaryExprAst>(std::move($1), std::move($3), Sclp::AbOp::MULT, @$);}
    | exp DIV exp                                   {$$ = std::make_unique<Sclp::ArithmeticBinaryExprAst>(std::move($1), std::move($3), Sclp::AbOp::DIV, @$);}
    | MINUS exp %prec UMINUS                        {$$ = std::make_unique<Sclp::ArithmeticUnaryExprAst>(std::move($2), Sclp::AuOp::UMINUS, @$);}
    | LEFT_ROUND_BRACKET exp RIGHT_ROUND_BRACKET    {$$ = std::move($2);}
    | exp QUESTION_MARK exp COLON exp               {$$ = std::make_unique<Sclp::ConditionalExprAst>(std::move($1), std::move($3), std::move($5), @$);}
    | exp AND exp                                   {$$ = std::make_unique<Sclp::BooleanBinaryExprAst>(std::move($1), std::move($3), Sclp::BbOp::AND, @$);}
    | exp OR exp                                    {$$ = std::make_unique<Sclp::BooleanBinaryExprAst>(std::move($1), std::move($3), Sclp::BbOp::OR, @$);}

    | NOT exp                                       {$$ = std::make_unique<Sclp::BooleanUnaryExprAst>(std::move($2), Sclp::BuOp::NOT, @$);}
    | rel_exp                                       {$$ = std::move($1);}
    | var_as_operand                                {$$ = std::move($1);}
    | const_as_operand                              {$$ = std::move($1);}
;

rel_exp
    : exp LESS_THAN exp                             {$$ = std::make_unique<Sclp::RelationalBinaryExprAst>(std::move($1), std::move($3), Sclp::RbOp::LT, @$);}
    | exp LESS_THAN_EQUAL exp                       {$$ = std::make_unique<Sclp::RelationalBinaryExprAst>(std::move($1), std::move($3), Sclp::RbOp::LTE, @$);}
    | exp GREATER_THAN exp                          {$$ = std::make_unique<Sclp::RelationalBinaryExprAst>(std::move($1), std::move($3), Sclp::RbOp::GT, @$);}
    | exp GREATER_THAN_EQUAL exp                    {$$ = std::make_unique<Sclp::RelationalBinaryExprAst>(std::move($1), std::move($3), Sclp::RbOp::GTE, @$);}
    | exp NOT_EQUAL exp                             {$$ = std::make_unique<Sclp::RelationalBinaryExprAst>(std::move($1), std::move($3), Sclp::RbOp::NEQ, @$);}
    | exp EQUAL exp                                 {$$ = std::make_unique<Sclp::RelationalBinaryExprAst>(std::move($1), std::move($3), Sclp::RbOp::EQ, @$);}
;

var_as_operand
    : var_name                                      {$$ = std::move($1);}
;

var_name
    : NAME                                          {$$ = std::make_unique<Sclp::NameExprAst>(std::move($1)+"_", @$);}
;

const_as_operand
    : INT_NUM                                       {$$ = std::make_unique<Sclp::NumberExprAst<int>>($1, @$);}
    | FLOAT_NUM                                     {$$ = std::make_unique<Sclp::NumberExprAst<double>>($1, @$);}
    | STR_CONST                                     {$$ = std::make_unique<Sclp::StringExprAst>($1, @$);}
;

%%

void Sclp::Parser::error(const Sclp::location& l, const std::string& m){
    std::cerr << l<< ": "<<m<<std::endl;
}