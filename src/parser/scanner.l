%option c++
%option noyywrap yylineno
%option yyclass="Sclp::Scanner"
/* %option prefix="Sclp" */
%option outfile="scanner.cc"

%{
#include "y.tab.h"
#include "scanner.hh"
#include "driver.hh"
                                                           
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

Sclp::Scanner::Scanner(std::istream* in)
    : yyFlexLexer(in), show_tokens(false), token_file_stream(nullptr) {
        loc.initialize();
    }

Sclp::Scanner::~Scanner() {
    if (token_file_stream) {
        token_file_stream->close();
        delete token_file_stream;
    }
}

void Sclp::Scanner::set_show_tokens(bool show, std::string& file_name) {
    show_tokens = show;
    if (show){
        std::string tok_name = std::string(file_name) + ".toks";
        token_file_stream = new std::ofstream(tok_name);
        if (!token_file_stream->is_open()){
            std::cerr<<"Error: Cannot open file "<<tok_name<<std::endl;
            exit(1);
        }
    }
}

bool Sclp::Scanner::get_show_tokens() const {return show_tokens;}

void Sclp::Scanner::write_token(const char* name, const char* lexeme, int lineno){
    if (show_tokens && token_file_stream->is_open()){
        *token_file_stream << "\tToken Name: "<<name<<" \tLexeme: "<<lexeme<<" \t Lineno: "<<lineno<<std::endl;
    }
}

Sclp::Parser::symbol_type Sclp::Scanner::lex(){
    int token = yylex();
    loc.step();

    switch (token){
        case Sclp::Parser::token::INTEGER :
            return Sclp::Parser::make_INTEGER(loc);
        case Sclp::Parser::token::FLOAT :
            return Sclp::Parser::make_FLOAT(loc);
        case Sclp::Parser::token::BOOL :
            return Sclp::Parser::make_BOOL(loc);
        case Sclp::Parser::token::STRING :
            return Sclp::Parser::make_STRING(loc);
        case Sclp::Parser::token::VOID :
            return Sclp::Parser::make_VOID(loc);

        case Sclp::Parser::token::WRITE :
            return Sclp::Parser::make_WRITE(loc);
        case Sclp::Parser::token::READ :
            return Sclp::Parser::make_READ(loc);
        case Sclp::Parser::token::RETURN :
            return Sclp::Parser::make_RETURN(loc);

        case Sclp::Parser::token::IF :
            return Sclp::Parser::make_IF(loc);
        case Sclp::Parser::token::ELSE :
            return Sclp::Parser::make_ELSE(loc);
        case Sclp::Parser::token::WHILE :
            return Sclp::Parser::make_WHILE(loc);
        case Sclp::Parser::token::DO :
            return Sclp::Parser::make_DO(loc);

        case Sclp::Parser::token::LEFT_CURLY_BRACKET :
            return Sclp::Parser::make_LEFT_CURLY_BRACKET(loc);
        case Sclp::Parser::token::RIGHT_CURLY_BRACKET :
            return Sclp::Parser::make_RIGHT_CURLY_BRACKET(loc);
        case Sclp::Parser::token::LEFT_ROUND_BRACKET :
            return Sclp::Parser::make_LEFT_ROUND_BRACKET(loc);
        case Sclp::Parser::token::RIGHT_ROUND_BRACKET :
            return Sclp::Parser::make_RIGHT_ROUND_BRACKET(loc);

        case Sclp::Parser::token::COMMA :
            return Sclp::Parser::make_COMMA(loc);
        case Sclp::Parser::token::SEMICOLON :
            return Sclp::Parser::make_SEMICOLON(loc);

        case Sclp::Parser::token::ASSIGN_OP :
            return Sclp::Parser::make_ASSIGN_OP(loc);
        case Sclp::Parser::token::PLUS :
            return Sclp::Parser::make_PLUS(loc);
        case Sclp::Parser::token::MINUS :
            return Sclp::Parser::make_MINUS(loc);
        case Sclp::Parser::token::MULT :
            return Sclp::Parser::make_MULT(loc);
        case Sclp::Parser::token::DIV :
            return Sclp::Parser::make_DIV(loc);

        case Sclp::Parser::token::LESS_THAN :
            return Sclp::Parser::make_LESS_THAN(loc);
        case Sclp::Parser::token::LESS_THAN_EQUAL :
            return Sclp::Parser::make_LESS_THAN_EQUAL(loc);
        case Sclp::Parser::token::GREATER_THAN :
            return Sclp::Parser::make_GREATER_THAN(loc);
        case Sclp::Parser::token::GREATER_THAN_EQUAL :
            return Sclp::Parser::make_GREATER_THAN_EQUAL(loc);
        case Sclp::Parser::token::EQUAL :
            return Sclp::Parser::make_EQUAL(loc);
        case Sclp::Parser::token::NOT_EQUAL :
            return Sclp::Parser::make_NOT_EQUAL(loc);

        case Sclp::Parser::token::QUESTION_MARK :
            return Sclp::Parser::make_QUESTION_MARK(loc);
        case Sclp::Parser::token::COLON :
            return Sclp::Parser::make_COLON(loc);

        case Sclp::Parser::token::OR :
            return Sclp::Parser::make_OR(loc);
        case Sclp::Parser::token::AND :
            return Sclp::Parser::make_AND(loc);
        case Sclp::Parser::token::NOT :
            return Sclp::Parser::make_NOT(loc);

        case Sclp::Parser::token::NAME :
            return Sclp::Parser::make_NAME(std::string(yytext), loc);

        case Sclp::Parser::token::FLOAT_NUM :
            char* end;
            return Sclp::Parser::make_FLOAT_NUM(std::strtod(yytext,&end), loc);
        case Sclp::Parser::token::INT_NUM :
            return Sclp::Parser::make_INT_NUM(std::atoi(yytext), loc);
        case Sclp::Parser::token::STR_CONST :
            return Sclp::Parser::make_STR_CONST(std::string(yytext), loc);
        
        case Sclp::Parser::token::ERROR :
            return Sclp::Parser::make_ERROR(loc);
        case Sclp::Parser::token::YYEOF :
            return Sclp::Parser::make_YYEOF(loc);
        
        default: {
            std::cerr<<"Unknown Token: "<<token<<std::endl;
            return Sclp::Parser::make_ERROR(loc);
        }
    }
}

#define dump(name) write_token(#name, yytext, yylineno)
%}


DIGIT       [0-9]
ID          [a-zA-Z_][a-zA-Z0-9_]*
WHITESPACE  [ \t\r]
%%

"int"           {dump(INTEGER); return Parser::token::INTEGER;}
"float"         {dump(FLOAT); return Parser::token::FLOAT;}
"bool"          {dump(BOOL); return Parser::token::BOOL;}
"string"        {dump(STRING); return Parser::token::STRING;}
"void"          {dump(VOID); return Parser::token::VOID;}

"print"         {dump(WRITE); return Parser::token::WRITE;}
"read"          {dump(READ); return Parser::token::READ;}
"return"        {dump(RETURN); return Parser::token::RETURN;}

"if"            {dump(IF); return Parser::token::IF;}
"else"          {dump(ELSE); return Parser::token::ELSE;}
"while"         {dump(WHILE); return Parser::token::WHILE;}
"do"            {dump(DO); return Parser::token::DO;}

"{"             {dump(LEFT_CURLY_BRACKET); return Parser::token::LEFT_CURLY_BRACKET;}
"}"             {dump(RIGHT_CURLY_BRACKET); return Parser::token::RIGHT_CURLY_BRACKET;}
"("             {dump(LEFT_ROUND_BRACKET); return Parser::token::LEFT_ROUND_BRACKET;}
")"             {dump(RIGHT_ROUND_BRACKET); return Parser::token::RIGHT_ROUND_BRACKET;}

","             {dump(COMMA); return Parser::token::COMMA;}
";"             {dump(SEMICOLON); return Parser::token::SEMICOLON;}

"="             {dump(ASSIGN_OP); return Parser::token::ASSIGN_OP;}
"+"             {dump(PLUS); return Parser::token::PLUS;}
"-"             {dump(MINUS); return Parser::token::MINUS;}
"*"             {dump(MULT); return Parser::token::MULT;}
"/"             {dump(DIV); return Parser::token::DIV;}

"<"             {dump(LESS_THAN); return Parser::token::LESS_THAN;}
"<="            {dump(LESS_THAN_EQUAL); return Parser::token::LESS_THAN_EQUAL;}
">"             {dump(GREATER_THAN); return Parser::token::GREATER_THAN;}
">="            {dump(GREATER_THAN_EQUAL); return Parser::token::GREATER_THAN_EQUAL;}
"=="            {dump(EQUAL); return Parser::token::EQUAL;}
"!="            {dump(NOT_EQUAL); return Parser::token::NOT_EQUAL;}

"?"             {dump(QUESTION_MARK); return Parser::token::QUESTION_MARK;}
":"             {dump(COLON); return Parser::token::COLON;}

"||"            {dump(OR); return Parser::token::OR;}
"&&"            {dump(AND); return Parser::token::AND;}
"!"             {dump(NOT); return Parser::token::NOT;}

{ID}                    {dump(NAME); return Parser::token::NAME;}

{DIGIT}+\.{DIGIT}*     {dump(FLOAT_NUM); return Parser::token::FLOAT_NUM;}
{DIGIT}*\.{DIGIT}+     {dump(FLOAT_NUM); return Parser::token::FLOAT_NUM;}
{DIGIT}+               {dump(INT_NUM); return Parser::token::INT_NUM;}
\"[^"]*\"              {dump(STR_CONST); return Parser::token::STR_CONST;}

{WHITESPACE}+           {loc.step();}
\n                      {loc.lines(yyleng); loc.step();}
"//".*                  {}
.                       {std::cerr<<"Unrecognised character on "<<yylineno<<":"<<yytext[0]; exit(2);}

%%

int SclpFlexLexer::yylex(){
    std::cerr<<"WHAT IS HAPPENING? WHO CALLED ME?"<<std::endl;
    exit(10);   
}

int SclpFlexLexer::yywrap(){
    return 1;
}