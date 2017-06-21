###################################################
#........../\./\...___......|\.|..../...\.........#
#........./..|..\/\.|.|_|._.|.\|....|.c.|.........#
#......../....../--\|.|.|.|i|..|....\.../.........#
#        Mathtin (c)                              #
###################################################
#   Author: Daniil [Mathtin] Shigapov             #
#   Copyright (c) 2017 <wdaniil@mail.ru>          #
#   This file is released under the MIT license.  #
###################################################
# Project: Ma.ZIP

CPP      = g++ #-pg#-DDEBUG
CC       = gcc #-pg#-DDEBUG
OBJ      = main.o arbasecoder.o arencoder.o ardecoder.o mafile.o \
           mazip.o mfttransform.o bwtransform.o
LINKOBJ  = $(OBJ) -lstdc++fs

CPPW     = x86_64-w64-mingw32-g++.exe #-pg#-DDEBUG
CCW      = x86_64-w64-mingw32-gcc.exe #-pg#-DDEBUG
OBJW     = main.obj arbasecoder.obj arencoder.obj ardecoder.obj mafile.obj \
           mazip.obj mfttransform.obj bwtransform.obj
LINKOBJW = $(OBJW)

LIBS     = 
INCS     = 
CXXINCS  = 
BIN      = mazip
CXXFLAGS = $(CXXINCS) -std=c++17 -Wall -pedantic -O3
CFLAGS   = $(INCS)    -std=c11   -Wall -pedantic -O3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) $(OBJ) $(OBJW) $(BIN) $(BIN).exe

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

$(BIN).exe: $(OBJW)
	$(CPPW) $(LINKOBJW) -o $(BIN).exe $(LIBS)

main.o: main.cpp mazip.h platform.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

mazip.o: mazip.cpp mazip.h arencoder.h ardecoder.h mafile.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

mafile.o: mafile.cpp mafile.h mazipinc.h mazipint.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

arbasecoder.o: arbasecoder.cpp arbasecoder.h mazipinc.h mazipint.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

arencoder.o: arencoder.cpp arencoder.h mazipinc.h arbasecoder.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

ardecoder.o: ardecoder.cpp ardecoder.h mazipinc.h arbasecoder.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

mfttransform.o: mfttransform.cpp mfttransform.h mazipinc.h mazipint.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

bwtransform.o: bwtransform.cpp bwtransform.h mazipinc.h mafile.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

main.obj: main.cpp mazip.h platform.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

mazip.obj: mazip.cpp mazip.h arencoder.h ardecoder.h mafile.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

mafile.obj: mafile.cpp mafile.h mazipinc.h mazipint.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

arbasecoder.obj: arbasecoder.cpp arbasecoder.h mazipinc.h mazipint.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

arencoder.obj: arencoder.cpp arencoder.h mazipinc.h arbasecoder.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

ardecoder.obj: ardecoder.cpp ardecoder.h mazipinc.h arbasecoder.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

mfttransform.obj: mfttransform.cpp mfttransform.h mazipinc.h mazipint.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)

bwtransform.obj: bwtransform.cpp bwtransform.h mazipinc.h mafile.h
	$(CPPW) -c $< -o $@ $(CXXFLAGS)
