#ifndef TAC_PRINTER_HH
#define TAC_PRINTER_HH

#include <string>
#include <memory>
#include "program.hh"

namespace Sclp{

void printTAC(std::shared_ptr<Program> program, std::string tac_print_file);

}
#endif