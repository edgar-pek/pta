
CC   := gcc
CXX  := g++
LD   := g++
CP   := cp

MINISAT_INCLUDE := /home/edgar/minisat/
LLVM_INCLUDE := /home/edgar/llvm/include/

MODULES := \
	$(MINISAT_INCLUDE)core \
	$(MINISAT_INCLUDE)mtl \
	$(MINISAT_INCLUDE)simp \
	$(MINISAT_INCLUDE)utils 


OPTFLAGS  := -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -D __STDC_CONSTANT_MACROS

all : CFLAGS   += -Wall -Wno-unused-function $(OPTFLAGS) -I $(LLVM_INCLUDE)
# Optimized version:
opt : CFLAGS   += -march=native -mtune=native -O6 -DOPTIMIZE -DNDEBUG -Wall -Wno-unused-function $(OPTFLAGS) -I $(LLVM_INCLUDE)
# Version to include MiniSat solver
# CFLAGS   += -Wall -Wno-unused-function $(OPTFLAGS) $(patsubst %, -I%, $(MODULES)) 
CXXFLAGS += $(CFLAGS) 
#CXXFLAGS += -std=c++0x $(CFLAGS) 

#LIBS := -lminisat
# this is needed for SparseBitVector support for dumping
LIBS := libLLVMSupport.a libLLVMSystemZInfo.a -pthread -ldl -lbdd
#LIBS := libLLVMSupport.a libLLVMSystem.a -pthread -ldl

all : main
opt : main

SRC := $(wildcard '*.cc')
HDR := $(wildcard '*.h')
# my modules
OBJ := Timer.o     \
			 parser.o	   \
			 CTGraph.o   \
			 Var.o		   \
			 Heap.o	     \
			 HeapField.o \
			 ParsePAG.o \
			 DumpDOOP.o \
			 Analyzer.o  \
			 TestBdd.o \
			 main.o

#			 SmtEncoding.o \
#			 TestSmallSet.o \
#			 TestSparseBitVector.o \
#			 TestStringMap.o \

# compile all .cc files
%.o : %.cc %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# link all *.o files
main : $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)
#	$(CXX) -o $@ $^

# clean 
clean :
	rm -f *.o main

#.PHONY: tags clean docs
#include $(patsubst %, %/module.make, $(MODULES))
#OBJ := \
#	$(patsubst %.cc, %.o, $(filter %.cc, $(SRC))) \
#	$(patsubst %.c, %.o,  $(filter %.c, $(SRC)))  \
#	$(patsubst %.y, %.o,  $(filter %.y, $(SRC))) 

#DEP := $(OBJ:.o=.d)
# implicit rules
#%.d: %.c
#	./depends.sh $(CC) `dirname $*.c` $(CFLAGS) $*.c > $@
#%.d: %.cc
#	./depends.sh $(CXX) `dirname $*.cc` $(CXXFLAGS) $(CFLAGS) $*.cc > $@
#-include $(DEP)

# Actual targets
#depend: $(DEP)
#clean: 
#	rm -rf $(PROG) lib$(PROG).a $(OBJ) $(GARBAGE) $(OBJ:.o=.d) 

tags:
	ctags -R .

#$(PROG): $(OBJ)
#	$(LD) -o $@ $^ $(LIBS)
#lib$(PROG).a: $(OBJ)
#	ar rv $@ $?
#	ranlib $@

#docs:
#	doxygen doxygen.conf
