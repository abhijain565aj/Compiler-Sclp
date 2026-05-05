#ifndef OPT_HANDLER_H
#define OPT_HANDLER_H

#include <argp.h>

struct Arguments {
    char* file_name;
    int sa_scan;
    int sa_parse;
    int sa_ast;
    int sa_tac;
    int sa_rtl;
    int show_tokens;
    int show_ast;
    int show_tac;
    int show_rtl;
    int show_symtab;
    int show_asm;
};

enum class opts{
    OPT_SA_SCAN=1000,
    OPT_SA_PARSE,
    OPT_SA_AST,
    OPT_SA_TAC,
    OPT_SA_RTL,
    OPT_SHOW_TOKENS,
    OPT_SHOW_AST,
    OPT_SHOW_TAC,
    OPT_SHOW_RTL,
    OPT_SHOW_SYMTAB,
    OPT_SHOW_ASM,
    OPT_SUPPRESS_COMMENTS
};

static struct argp_option options[] = {
    {"sa-scan", static_cast<int>(opts::OPT_SA_SCAN), 0, 0, "Stop after scanning", 0},
    {"sa-parse", static_cast<int>(opts::OPT_SA_PARSE), 0, 0, "Stop after parsing", 1},
    {"sa-ast", static_cast<int>(opts::OPT_SA_AST), 0, 0, "Stop after constructing Abstract Syntax Tree (AST)", 2},    
    {"sa-tac", static_cast<int>(opts::OPT_SA_TAC), 0, 0, "Stop after constructing Three Address Code (TAC)", 3},    
    {"sa-rtl", static_cast<int>(opts::OPT_SA_RTL), 0, 0, "Stop after constructing Register Transfer Language (RTL) code", 4},    
    {"show-tokens", static_cast<int>(opts::OPT_SHOW_TOKENS), 0, 0, "Show the tokens in FILE.toks (or out.toks)", 5},
    {"show-ast", static_cast<int>(opts::OPT_SHOW_AST), 0, 0, "Show abstract syntax trees in FILE.ast (or out.ast)", 6},
    {"show-tac", static_cast<int>(opts::OPT_SHOW_TAC), 0, 0, "Show the Three Address Code in FILE.tac (or out.tac)", 7},
    {"show-rtl", static_cast<int>(opts::OPT_SHOW_RTL), 0, 0, "Show the Register Transfer Language code in FILE.rtl (or out.rtl)", 8},
    {"show-symtab", static_cast<int>(opts::OPT_SHOW_SYMTAB), 0, 0, "Show the symbol table after RTL construction (when offsets are allocated) in FILE.sym, (or out.sym)", 9},
    {"show-asm", static_cast<int>(opts::OPT_SHOW_ASM), 0, 0, "Generate the assembly program in FILE.spim (or out.spim). This is the default action and is suppressed only if a valid `sa-...' option is given to stop the compilation after some earlier phase.", 10},
    {"suppress-comments", static_cast<int>(opts::OPT_SUPPRESS_COMMENTS), 0, 0, "Suppress comments in RTL and Assembly files ", 14},
    {}
};

static error_t parse_opt(int key, char* arg, argp_state *state){
    struct Arguments *arguments = static_cast<struct Arguments*>(state->input);

    switch (key) {
        case static_cast<int>(opts::OPT_SA_SCAN):
            arguments->sa_scan = 1;
            break;
        
        case static_cast<int>(opts::OPT_SA_PARSE):
            arguments->sa_parse = 1;
            break;
        
        case static_cast<int>(opts::OPT_SA_AST):
            arguments->sa_ast = 1;
            break;

        case static_cast<int>(opts::OPT_SA_TAC):
            arguments->sa_tac = 1;
            break;

        case static_cast<int>(opts::OPT_SA_RTL):
            arguments->sa_rtl = 1;
            break;
        
        case static_cast<int>(opts::OPT_SHOW_TOKENS):
            arguments->show_tokens = 1;
            break;
        
        case static_cast<int>(opts::OPT_SHOW_AST):
            arguments->show_ast = 1;
            break;
        
        case static_cast<int>(opts::OPT_SHOW_TAC):
            arguments->show_tac = 1;
            break;
        
        case static_cast<int>(opts::OPT_SHOW_RTL):
            arguments->show_rtl = 1;
            break;
        
        case static_cast<int>(opts::OPT_SHOW_SYMTAB):
            arguments->show_symtab = 1;
            break;

        case static_cast<int>(opts::OPT_SHOW_ASM):
            arguments->show_asm = 1;
            break;

        case static_cast<int>(opts::OPT_SUPPRESS_COMMENTS):
            break;

        case ARGP_KEY_ARG:
            if (arguments->file_name == nullptr){
                arguments->file_name = arg;
            } else {
                argp_error(state, "Only one input file allowed");
            }
            break;
        case ARGP_KEY_END:
            if (!arguments->file_name){
                argp_error(state, "No input file specified");
            }
            break;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, "FILE", "Sclp - A language processor for C-like language",0,0,0};

#endif