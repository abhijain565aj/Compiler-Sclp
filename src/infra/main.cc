#include <string>
#include <assert.h>

#include "argument_handler.hh"
#include "driver.hh"

int main(int argc, char * argv[]) {
    // Process the command line options
    Arguments args = {};
    argp_parse(&argp, argc, argv, 0, 0, &args);

    Sclp::Driver driver;
    driver.show_tokens = args.show_tokens;
    driver.show_ast = args.show_ast;
    driver.show_tac = args.show_tac;
    driver.show_rtl = args.show_rtl;
    driver.show_symtab = args.show_symtab;
    driver.show_asm = args.show_asm || true; // produce assembly file irrespective of flag
    driver.open_file(args.file_name);
    driver.initScanner();
    driver.initParser();

    int status = 0;
   
    if (args.sa_scan){
        status = driver.scan_only();
        return status;
    }
   
    status = status || driver.parse_only();
    if (args.sa_parse)
        return status;
    
    status = status || driver.ast_only();
    if(args.sa_ast)
        return status;
    
    status = status || driver.tac_only();
    if(args.sa_tac)
        return status;

    status = status || driver.rtl_only();
    if(args.sa_rtl)
        return status;

    status = status || driver.asm_only();
    return status;
}
