HEADERS := $(wildcard *.h)
SRC := RuleByDocPS.cpp Infltrait.cpp  PPMV.cpp TermByDoc.cpp  Trie.cpp  \
	RuleByDoc.cpp SuffixCollector.cpp # NamedMatrix.cpp

DBG_OBJ = ${SRC:%.cpp=$(DBG)/%.o}
OPT_OBJ = ${SRC:%.cpp=$(OPT)/%.o}

LIBNAME := libinflds

include Makefile.inc

LDFLAGS := $(LDFLAGS) -linflio -linflsd # -linflla

.PHONY: dbg opt test

.SECONDEXPANSION:

dbg: $(SHARED)

include Makefile.deps

# $(DBG)/%.o: %.cpp $(HEADERS) $$(@D)/.f
# 	$(CXX) $(CFLAGS) $(CDEBUG) -c $< -o $@

opt: $(SHARED)

$(OPT)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(COPT) -c $< -o $@

$(LIBNAME).a: $(OBJ)
	${AR} $@ $^

$(LIBDIR)/$(LIBNAME).so: $(OBJ) $$(@D)/.f
	${CXX} $(DYLIBFLAGS) -shared -Wl,-soname,$@ -o $@ $(filter %.o,$^) $(LDFLAGS)

$(LIBDIR)/$(LIBNAME).dylib: $(OBJ) $$(@D)/.f
	${CXX} $(DYLIBFLAGS) -dynamiclib -o $@ $(filter %.o,$^) $(LDFLAGS)

test: $(OBJ)
	$(CXX) test.cpp $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS)

.PHONY: cleanall
cleanall:
	-rm -rf $(DBG) $(OPT) test test.dSYM
	-rm $(SHARED)

.PRECIOUS: %/.f
%/.f:
	-mkdir -p $(dir $@)
	-touch $@

