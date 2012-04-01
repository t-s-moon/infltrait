LIBNAME := libinflla

HEADERS := $(wildcard *.h) #Matrix.h Vector.h MiniGSL.h Op.h
SRC := MiniGSL.cpp Op.cpp

SRC := $(SRC) #schone.cpp
OPT_OBJ = ${SRC:%.cpp=$(OPT)/%.o}
DBG_OBJ = ${SRC:%.cpp=$(DBG)/%.o}

include Makefile.inc

.PHONY: dbg opt clean cleanall
.SECONDEXPANSION:

dbg: $(SHARED) #test.dbg schone.dbg

test.dbg: $(TST_OBJ_DBG)
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS) 

schone.dbg: $(SCH_OBJ_DBG)
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ $^ $(LDFLAGS) 

$(DBG)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(CDEBUG) -o $@ -c $< 

opt: $(SHARED) #schone

schone: $(SCH_OBJ)
	$(CXX) $(CFLAGS) $(COPT) $(LDFLAGS) -o $@ $^

$(OPT)/%.o: %.cpp $(HEADERS) $$(@D)/.f
	$(CXX) $(CFLAGS) $(COPT) -o $@ -c $< 

mini: gzlib
	$(CXX) $(CFLAGS) $(COPT) extract_giga.cpp ../gzstream/libgzstream.a -o $@ -lz

gzlib:
	$(MAKE) -C ../gzstream

$(LIBNAME).a: $(OBJ)
	${AR} $@ $^

$(LIBDIR)/$(LIBNAME).so: $(OBJ) $$(@D)/.f
	${CXX} $(DYLIBFLAGS) -shared -Wl,-soname,$@ -o $@ $(filter %.o,$^) $(LDFLAGS)

$(LIBDIR)/$(LIBNAME).dylib: $(OBJ) $$(@D)/.f
	${CXX} $(DYLIBFLAGS) -dynamiclib -o $@ $(filter %.o,$^) $(LDFLAGS)

clean:
	-rm -f *.dbg schone

cleanall:
	-rm -rf $(OPT)/{*,.f} $(DBG)/{*,.f} *.dbg schone $(SHARED)
	-rm -rf test test.dSYM
	-rmdir $(OPT) $(DBG)

.PRECIOUS: %/.f
%/.f:
	mkdir -p $(dir $@)
	touch $@


