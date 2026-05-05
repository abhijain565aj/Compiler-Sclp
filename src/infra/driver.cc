#include <iostream>
#include <sstream>
#include <assert.h>

#include "y.tab.h"
#include "scanner.hh"
#include "driver.hh"
#include "ast_type_check.hh"
#include "ast_printer.hh"
#include "tac_builder.hh"
#include "tac_printer.hh"
#include "rtl_builder.hh"
#include "rtl_printer.hh"
#include "asm_builder.hh"
#include "asm_printer.hh"

namespace Sclp{
Driver::Driver(): show_tokens(false), show_ast(false), show_tac(false), show_rtl(false), show_asm(false), show_symtab(false), scanner(nullptr), parser(nullptr), file_stream(nullptr){
}

Driver::~Driver(){
}

void Driver::open_file(const std::string& file_name_){
    file_name = file_name_;
    file_stream = new std::ifstream(file_name);
    if (!file_stream->good()){
        error("Cannon open file: "+file_name);
    }
}

void Driver::initScanner(){
    assert(file_stream);

    scanner = std::make_shared<Scanner>(file_stream); 
    scanner->set_show_tokens(show_tokens, file_name);  
}

void Driver::initParser(){
    assert(scanner);
    
    parser = std::make_shared<Parser>(*scanner, *this);
}

void Driver::error(const location& loc, const std::string& m){
    std::cerr<<"Error: "<<m<<std::endl;
    std::cerr<<"Position "<<loc.begin<<" "<<loc.end<<std::endl;
    exit(1);
}

void Driver::error(const std::string& m){
    std::cerr<<"Error: "<<m<<std::endl;
    exit(1);
}

int Driver::scan_only(){
    assert(scanner);

    while (true){                   
        auto sym = scanner->lex();
        if (sym.kind() == Parser::symbol_kind::S_YYEOF){
            break;
        }
    }

    return 0;
}

int Driver::parse_only(){
    return parser->parse();
}

int Driver::ast_only(){
    if(!program)
        error("Program not initiated");
    
    std::shared_ptr<AstTypeCheck> checker = std::make_shared<AstTypeCheck>(program);
    if (show_ast){
        std::string ast_print_file = file_name + ".ast";
        std::shared_ptr<AstPrinter> printer = std::make_shared<AstPrinter>(program, ast_print_file);
    }
    return 0;
}

int Driver::tac_only(){
    std::shared_ptr<TacBuilder> tacBuilder = std::make_shared<TacBuilder>(program);
    if(show_tac){
        std::string tac_print_file = file_name + ".tac";
        printTAC(program, tac_print_file);
    }
    return 0;
}

int Driver::rtl_only(){
    std::shared_ptr<RtlBuilder> rtlBuilder = std::make_shared<RtlBuilder>(program);

    if(show_rtl){
        std::string rtl_print_file = file_name + ".rtl";
        printRTL(program, rtl_print_file);
    }
    return 0;
}

int Driver::asm_only(){
    std::shared_ptr<AsmBuilder> asmBuilder = std::make_shared<AsmBuilder>(program);
    if(show_symtab){
        std::string symtab_print_file = file_name + ".sym";
        printSymTab(program,symtab_print_file);
    }
    if(show_asm){
        std::string asm_print_file = file_name + ".spim";
        printASM(program, asm_print_file);
    }
    return 0;
}
} //namespace Sclp