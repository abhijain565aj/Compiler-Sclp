#ifndef ASM_PRINTER_HH
#define ASM_PRINTER_HH

#include <string>
#include <memory>
#include "program.hh"

namespace Sclp{

void printASM(std::shared_ptr<Program> program, std::string asm_print_file);
void printSymTab(std::shared_ptr<Program> program, std::string symtab_print_file);

}
#endif