#ifndef SCANNER_HH
#define SCANNER_HH

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer SclpFlexLexer

#include <FlexLexer.h>
#include <fstream>
#include <string>

namespace Sclp{
class Parser;
}

namespace Sclp {

class Scanner : public yyFlexLexer {
public:
    Scanner(std::istream* in);
    virtual ~Scanner();

    virtual int yylex() override;
    Sclp::Parser::symbol_type lex();

    bool get_show_tokens() const;
    void set_show_tokens(bool show, std::string& file_name);
    void write_token(const char* name, const char* lexeme, int lineno);

private:
    bool show_tokens;
    std::ofstream* token_file_stream;
    Sclp::Parser::location_type loc;
};

}

#endif

