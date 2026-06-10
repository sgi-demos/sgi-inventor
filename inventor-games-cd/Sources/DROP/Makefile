#!smake

include /usr/include/make/commondefs

C++FILES = TsBlock.c++ TsEvents.c++ TsField.c++\
	TsPiece.c++ TsPieces.c++ TsRemoval.c++ TsTrickle.c++ TsViewer.c++ \
	drop.c++

PROGRAM  = drop

OPTIMIZER=-g

TARGETS  = $(PROGRAM)

# Libraries to link with:
LLDLIBS  =  -lInventorXt -lGL -lm -laudio -laudiofile 

LC++DEFS = -DDEBUG

default: $(TARGETS)

include $(COMMONRULES)

$(TARGETS): $(OBJECTS)
	$(C++F) -o $@ $(OBJECTS) $(LDFLAGS)

