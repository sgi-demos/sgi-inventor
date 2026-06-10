#!smake

ROOT =
PROGRAM = SpaceCadet

include /usr/include/make/commondefs

C++FILES = Change.c++ Grid.c++ Line.c++ Message.c++ Spec.c++ Solver.c++
OPTIMIZER = -O

TARGETS = pbnsolve pbncreate

# Libraries to link with:
LLDLIBS =   -lInventorXt

default: $(TARGETS)

include $(COMMONRULES)

$(TARGETS): $@.o $(OBJECTS)
	$(C++F) -o $@ $@.o $(OBJECTS) $(LDFLAGS)
