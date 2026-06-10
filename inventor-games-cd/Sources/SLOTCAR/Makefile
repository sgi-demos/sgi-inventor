#!smake

include /usr/include/make/commondefs

C++FILES = Car.c++ Simulation.c++ MouseInput.c++\
	Network.c++ Packet.c++ RaceCar.c++ Sound.c++ State.c++ \
	Track.c++ Curve.c++ Straight.c++ StartFinish.c++ \
	slotcar.c++ Sky.c++ RobotCar.c++ LODD.c++
# CFILES   = libsample.c
PROGRAM  = slotcar

OPTIMIZER=-g

TARGETS  = $(PROGRAM)

# Libraries to link with:
LLDLIBS  =  -lInventorXt -lGL -lgutil -lm -laudio -laudiofile 

LC++DEFS = -DDEBUG

default: $(TARGETS)

include $(COMMONRULES)

$(TARGETS): $(OBJECTS)
	$(C++F) -o $@ $(OBJECTS) $(LDFLAGS)

TestTrack: Track.o Straight.o Curve.o TestTrack.o
	$(C++F) -o $@ Track.o Straight.o Curve.o TestTrack.o $(LDFLAGS)

genForest: genForest.c++
	$(C++F) -o $@ genForest.c++ $(LDFLAGS)

genCrowd: genCrowd.c++
	$(C++F) -o $@ genCrowd.c++ $(LDFLAGS)
