# Compiler-Sclp

**Contributors:** Malay Kedia, Abhi Jain

This repository contains an implementation of **SCLP**, a language processor for a small C-like language, based on Prof. Uday Khedker's SCLP framework: <https://www.cse.iitb.ac.in/~uday/sclp-web/>.

This project was completed as part of **CS-306**. The final assignment after five labs is present in the `src/` directory, and the implementation supports SCLP up to **Level 5 / Assignment 5**.

## Build

The source files are organized inside the `src/` directory, while compilation uses hard symbolic links from a single directory. Therefore, whenever new files are added or changes are pulled from Git, recreate the links from inside `src/`:

```bash
cd src
make create-links
```

After setting up the links, build the final `sclp` executable from the `sclp-submit` directory:

```bash
cd ../sclp-submit
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
