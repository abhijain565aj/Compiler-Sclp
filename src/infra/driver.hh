#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <string>
#include <memory>
#include <fstream>

#include "program.hh"

namespace Sclp{
    class Scanner;
    class Parser;

    class Driver {
        public:
            std::shared_ptr<Program> program;

            Driver();
            ~Driver();

            void open_file(const std::string& file_name);

            int scan_only();
            int parse_only();
            int ast_only();
            int tac_only();
            int rtl_only();
            int asm_only();

            std::string file_name;
            bool show_tokens;
            bool show_ast;
            bool show_tac;
            bool show_rtl;
            bool show_asm;
            bool show_symtab;

            void initScanner();
            void initParser();

        private:
            void error(const class location& l, const std::string& m);
            void error(const std::string& m);

            std::shared_ptr<Scanner> scanner;
            std::shared_ptr<Parser> parser;

            std::ifstream* file_stream;
    };
}

#endif