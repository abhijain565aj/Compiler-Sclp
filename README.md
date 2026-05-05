# Compiler-Sclp

This repository contains an implementation of **SCLP**, a language processor for a small C-like language, based on Prof. Uday Khedker's SCLP framework: <https://www.cse.iitb.ac.in/~uday/sclp-web/>.

The project was done as part of **CS-306**. The implementation in `src/` contains the final assignment after 5 labs and supports SCLP up to **Level 5 / Assignment 5**.

## Build

```bash
cd src
make
```

## Usage

```bash
./sclp [OPTION...] FILE
```

Common options:

```text
--sa-scan           Stop after scanning
--sa-parse          Stop after parsing
--sa-ast            Stop after constructing AST
--sa-tac            Stop after constructing TAC
--sa-rtl            Stop after constructing RTL code
--show-tokens       Show generated tokens
--show-ast          Show abstract syntax tree
--show-tac          Show three-address code
--show-rtl          Show RTL code
--show-symtab       Show symbol table
--show-asm          Generate SPIM assembly
--suppress-comments Suppress comments in RTL and assembly files
```

## Clean

```bash
make clean
```
