#ifndef RTL_PRINTER_HH
#define RTL_PRINTER_HH

#include <string>
#include <memory>
#include "program.hh"

namespace Sclp{

void printRTL(std::shared_ptr<Program> program, std::string rtl_print_file);

}
#endif