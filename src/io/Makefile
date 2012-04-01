HEADERS := $(wildcard *.h)
SRC := AnsiCType.cpp  IO.cpp

DBG_OBJ = ${SRC:%.cpp=$(DBG)/%.o}
OPT_OBJ = ${SRC:%.cpp=$(OPT)/%.o}

LIBNAME := libinflio

include Makefile.inc

LDFLAGS := $(LDFLAGS) #-linflla

.PHONY: dbg opt default

.SECONDEXPANSION:

dbg: $(DBG_OBJ) $(SHARED)

$(DBG)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(CDEBUG) -c $< -o $@

opt: $(OPT_OBJ) $(SHARED)

$(OPT)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(COPT) -c $< -o $@

$(LIBNAME).a: $(OBJ)
	${AR} $@ $^

$(LIBDIR)/$(LIBNAME).so: $(OBJ) $$(@D)/.f
	${CXX} $(DYLIBFLAGS) -shared -Wl,-soname,$@ -o $@ $(filter %.o,$^) $(LDFLAGS)

$(LIBDIR)/$(LIBNAME).dylib: $(OBJ) $$(@D)/.f
	${CXX} $(DYLIBFLAGS) -dynamiclib -o $@ $(filter %.o,$^) $(LDFLAGS)

test:
	$(MAKE) -f tmake

.PHONY: cleanall
cleanall:
	-rm -rf $(DBG) $(OPT) test test.dSYM
	-rm $(SHARED)

.PRECIOUS: %/.f
%/.f:
	-mkdir -p $(dir $@)
	-touch $@

