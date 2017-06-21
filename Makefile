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

BIN      = mazip
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) $(BIN) $(BIN).exe
	cd src && make clean

src/%.o:
	cd src && make $@

$(BIN): src/*
	cd src && make $(BIN)
	cp src/$(BIN) .

$(BIN).exe: src/*
	cd src && make $(BIN).exe
	cp src/$(BIN).exe .
