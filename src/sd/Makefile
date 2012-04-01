HEADERS := $(wildcard *.h)
SRC := LCS.cpp Patricia.cpp

DBG_OBJ = ${SRC:%.cpp=$(DBG)/%.o}
OPT_OBJ = ${SRC:%.cpp=$(OPT)/%.o}

LIBNAME := libinflsd

include Makefile.inc

LDFLAGS := $(LDFLAGS)

.PHONY: dbg opt test

.SECONDEXPANSION:

dbg: $(SHARED)

$(DBG)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(CDEBUG) -c $< -o $@

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

