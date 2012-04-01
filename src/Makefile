include Makefile.inc

################################################################################
# Definition of main driver objects and sources
################################################################################
HEADERS := Eval.h
SRC := Eval.cpp Driver.cpp

OBJ := ${SRC:%.cpp=$(OPT)/%.o}
DBG_OBJ := ${SRC:%.cpp=$(DBG)/%.o}

################################################################################
# Definition of EvalCelex objects and sources
################################################################################
EVAL_SRC := Eval.cpp EvalCelex.cpp
EVAL_OBJ := ${EVAL_SRC:%.cpp=$(OPT)/%.o}
EVAL_DBG_OBJ := ${EVAL_SRC:%.cpp=$(DBG)/%.o}

################################################################################
# Definition of Trie printing objects and sources
################################################################################
PT_SRC := PrintTrie.cpp
PT_OBJ := ${PT_SRC:%.cpp=$(OPT)/%.o}
PT_DBG_OBJ := ${PT_SRC:%.cpp=$(DBG)/%.o}

################################################################################
# Definition of test file objects and sources
################################################################################
TEST_SRC := test.cpp
TEST_OBJ := ${TEST_SRC:%.cpp=$(OPT)/%.o}
TEST_DBG_OBJ := ${TEST_SRC:%.cpp=$(DBG)/%.o}

################################################################################
# Definition of global compiler and linker variables and options
################################################################################
PREFIX := $(abspath $(shell pwd)/..)
INCLUDEDIR := $(PREFIX)/src
CFLAGS := -Wall -I$(INCLUDEDIR)

LIBDIR 	:= $(PREFIX)/lib
LDFLAGS :=  -L$(LIBDIR) #-L$(LIBATLAS) -llapack -lcblas -latlas -lz -lgzstream
LDFLAGS := $(LDFLAGS) -linflds -linflio -linflsd  #-linflla

################################################################################
# Target body
################################################################################

EXES := driver eval_celex print_trie test
DBG_EXES := $(addsuffix .dbg, $(EXES))

.PHONY: dbg opt 

.SECONDEXPANSION:

default: libraries.dbg $(DBG_EXES) # driver.dbg eval_celex.dbg print_trie.dbg test.dbg

libraries.dbg:
	$(MAKE) -C gzstream
	$(MAKE) -C io
	$(MAKE) -C sd
	$(MAKE) -C ml
	$(MAKE) -C ds
#	$(MAKE) -C la

driver.dbg: $(DBG_OBJ)
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS)

eval_celex.dbg: $(EVAL_DBG_OBJ)
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS)

test.dbg:  $(TEST_DBG_OBJ)
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS)

print_trie.dbg:  $(PT_DBG_OBJ)
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS)

opt: libraries $(EXES)

libraries:
	$(MAKE) -C gzstream
	$(MAKE) -C io opt
	$(MAKE) -C sd opt
#	$(MAKE) -C ml opt
	$(MAKE) -C ds opt
#	$(MAKE) -C la opt

driver: $(OBJ)
	$(CXX) $(CFLAGS) $(COPT) -o $@ $^ $(LDFLAGS)

eval_celex: $(EVAL_OBJ)
	$(CXX) $(CFLAGS) $(COPT) -o $@ $^ $(LDFLAGS)

print_trie: $(PT_OBJ)
	$(CXX) $(CFLAGS) $(COPT) -o $@ $^ $(LDFLAGS)

test: $(TEST_OBJ)
	$(CXX) $(CFLAGS) $(COPT) -o $@ $^ $(LDFLAGS)

$(DBG)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ -c $< 

$(OPT)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(COPT) -o $@ -c $<

%/.f:
	-mkdir -p $(dir $@)
	-touch $@

.PHONY: clean cleanall
clean:
	-rm -f $(DBG_EXES) $(EXES)

cleanall:
	-rm -f $(DBG_EXES) $(EXES) $(OPT)/{*,.f} $(DBG)/{*,.f}
	-rm -rf *.dSYM
	-rmdir $(OPT) $(DBG)
	$(MAKE) -C gzstream cleanall
	$(MAKE) -C la cleanall
	$(MAKE) -C io cleanall
	$(MAKE) -C ml cleanall
	$(MAKE) -C ds cleanall
	$(MAKE) -C sd cleanall

.PRECIOUS: %/.f

