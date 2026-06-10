#!smake

C++FILES = SoSocket.c++ Player.c++ Server.c++ Msg.c++ \
	MovingObject.c++ Rink.c++ Simulation.c++ main.c++ \
	Sound.c++

PROGRAM = puck

# /usr/include/make/commondefs and /usr/include/make/commonrules
# define some useful Makefile rules.  For example, they
# defines a 'clean' target, so you can type 'make clean'
# to remove all .o files (or other files generated during
# compilation). See the file /usr/include/make/commonrules for
# documentation on what targets are supported.

include /usr/include/make/commondefs

TARGETS = $(PROGRAM)

# Libraries to link with:
LLDLIBS =  -lInventorXt -lGL -laudio -laudiofile -lX11 -lm

#LC++DEFS = -g

default: $(TARGETS)

include $(COMMONRULES)

$(TARGETS): $(OBJECTS)
	$(C++F) -o $@ $(OBJECTS) $(LDFLAGS)

