HEADERS := KMeans.h EM.h Simplex.h ChiSquareTest.h
SRC := #ChiSquareTest.cpp

DBG_OBJ = ${SRC:%.cpp=$(DBG)/%.o}
OPT_OBJ = ${SRC:%.cpp=$(OPT)/%.o}

LIBNAME := libinflml

include Makefile.inc

.PHONY: dbg opt

.SECONDEXPANSION:

dbg: $(DBG_OBJ) # $(SHARED)

$(DBG)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(CDEBUG) -c $< -o $@

opt: $(OPT_OBJ) $(SHARED)

$(OPT)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(COPT) -c $< -o $@

$(LIBNAME).a: $(OBJ)
	${AR} $@ $^

$(LIBDIR)/$(LIBNAME).so: $(OBJ)
	${CXX} -shared -Wl,-soname,$@ -o $@  $<

$(LIBDIR)/$(LIBNAME).dylib: $(OBJ)
	${CXX} -dynamiclib $< -o $@

.PHONY: cleanall
cleanall:
	-rm -rf $(DBG) $(OPT)
	-rm $(SHARED)

.PRECIOUS: %/.f
%/.f:
	-mkdir -p $(dir $@)
	-touch $@

