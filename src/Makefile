GROUP_ID=JAN2026_group_7

CXX = g++-15
# Combined CXXFLAGS so the strict warnings are applied alongside the C++23 and MMD flags
# CXXFLAGS = -std=c++23 -g -Wall -Wextra -Wsign-conversion -Wconversion -Wpedantic -Wshadow -Wnon-virtual-dtor -Wsuggest-override -Wmissing-declarations -Wnull-dereference -Woverloaded-virtual -Wlogical-op -Wduplicated-cond -Wduplicated-branches -MMD

TARGET = sclp

# --- OS-SPECIFIC SETUP ---
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS Configuration (Local machine via Homebrew)
    FLEX_PREFIX := $(shell brew --prefix flex)
    ARGP_PREFIX := $(shell brew --prefix argp-standalone)
    BISON_PREFIX := $(shell brew --prefix bison)

    # Add the include paths for the compiler
    CXXFLAGS += -I$(FLEX_PREFIX)/include -I$(ARGP_PREFIX)/include
    
    # Add the library path and specific argp library for the linker
    LDFLAGS += -L$(ARGP_PREFIX)/lib -largp
    
    # Override default Apple tools with modern Homebrew versions
    LEX = $(FLEX_PREFIX)/bin/flex
    YACC = $(BISON_PREFIX)/bin/bison
else
    # Linux Configuration (Grading server)
    LEX = flex
    YACC = bison
    
    # argp is built into glibc on Linux, so we leave LDFLAGS empty here
    LDFLAGS += 
endif
# -------------------------

LOCATION_FILE = location.hh
LEX_SRC = scanner.l
YACC_SRC = parser.y

LEX_OUT_C = scanner.cc
YACC_OUT_C = parser.tab.cc
YACC_OUT_H = y.tab.h

OBJS = main.o scanner.o parser.tab.o driver.o program.o ast_printer.o ast_type_check.o tac_builder.o tac.o tac_printer.o rtl_builder.o rtl.o rtl_printer.o rtl_registers.o asm.o asm_builder.o asm_printer.o

DEPS = $(OBJS:.o=.d)

TC_DIR = ../example-programs/
SETUP-DIR = ../sclp/

all: $(TARGET)

# The LDFLAGS variable is added to the very end of this linking step
$(TARGET): $(YACC_OUT_H) $(YACC_OUT_C) $(LEX_OUT_C) $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(YACC_OUT_H) $(YACC_OUT_C) : $(YACC_SRC)
	$(YACC) -dv -o $(YACC_OUT_C) --defines=$(YACC_OUT_H) -Wcounterexamples $(YACC_SRC)
	mv parser.output y.output

$(LEX_OUT_C): $(LEX_SRC) $(YACC_OUT_H)
	$(LEX) --c++ --outfile=$(LEX_OUT_C) $(LEX_SRC)

-include $(DEPS)

clean-log:
	@-find $(TC_DIR) -name "*.c.toks" -delete
	@-find $(TC_DIR) -name "*.c.ast" -delete
	@-find $(TC_DIR) -name "*.c.tac" -delete
	@-find $(TC_DIR) -name "*.c.rtl" -delete
	@-find $(TC_DIR) -name "*.c.spim" -delete
	@-find $(TC_DIR) -name "*.c.sym" -delete
	@-rm -f test.c
	@-rm -rf ../$(GROUP_ID)
	@-rm -f ../$(GROUP_ID).tar.gz

clean:
	@-rm -f $(TARGET) $(OBJS) $(DEPS) $(YACC_OUT_C) $(YACC_OUT_H) $(LEX_OUT_C) $(LOCATION_FILE) 
	@-rm -f *.log *.toks *.output *.ast *.tac *.rtl *.spim *.sym

submit: clean clean-log
	mkdir -p ../$(GROUP_ID)
	cp * ../$(GROUP_ID)/
	rm -f ../$(GROUP_ID)/*.sh
	rm -f ../$(GROUP_ID)/*.py
	tar -cvzf ../$(GROUP_ID).tar.gz ../$(GROUP_ID)/ 2>/dev/null

create-links:
	@bash create.sh

run:
	@bash checker.sh -c -v