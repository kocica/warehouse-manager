###############################################################
# @file    Makefile                                           #
# @author  Filip Kocica                                       #
# @date    09/30/2020                                         #
#                                                             #
# Warehouse manager makefile                                  #
###############################################################

################## Build constants ##############
BIN_NAME_SIM = whm_sim
BIN_NAME_GEN = whm_gen
BIN_NAME_GUI = whm_gui
BIN_NAME_OPT = whm_opt
BIN_NAME_PAF = whm_paf

README     = README

QMAKE      = qmake
QFLAGS     = -qt=qt5

CC         = g++ #g++-8
CFLAGS     = -std=c++17 -pedantic -Wall -Wextra -O3 #-fopenmp

PROFILE    = #-pg

LDFLAGS    = -lsimlib -lm
LDLIBS     = 
INCLUDE    = 

SRC        = src
CFG        = cfg
GUI        = $(SRC)/gui

HEADERS    = $(wildcard $(SRC)/*.h)
SOURCES    = $(wildcard $(SRC)/*.cpp)
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))

GUI_SOURCES = $(wildcard $(GUI)/*.cpp)
GUI_HEADERS = $(wildcard $(GUI)/*.h)
GUI_OBJS    = $(patsubst %.cpp, %.o, $(GUI_SOURCES))

all:
	make $(BIN_NAME_GEN)
	make $(BIN_NAME_SIM)
	make $(BIN_NAME_OPT)
	make $(BIN_NAME_PAF)
	make $(BIN_NAME_GUI)

.PHONY: clean

$(BIN_NAME_OPT): CFLAGS += -DWHM_OPT -DWHM_SIM
$(BIN_NAME_OPT): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(PROFILE) -L$(LDLIBS) -Wl,-rpath=$(LDLIBS) -fPIC $(OBJS) $(LDFLAGS) -o $@
	find . -type f -name "*.cpp" -exec touch --no-create {} +

$(BIN_NAME_PAF): CFLAGS += -DWHM_PAF
$(BIN_NAME_PAF): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(PROFILE) -fPIC $(OBJS) -o $@
	find . -type f -name "*.cpp" -exec touch --no-create {} +

$(BIN_NAME_GEN): CFLAGS += -DWHM_GEN
$(BIN_NAME_GEN): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(PROFILE) -fPIC $(OBJS) -o $@
	find . -type f -name "*.cpp" -exec touch --no-create {} +

$(BIN_NAME_SIM): CFLAGS += -DWHM_SIM
$(BIN_NAME_SIM): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(PROFILE) -L$(LDLIBS) -Wl,-rpath=$(LDLIBS) -fPIC $(OBJS) $(LDFLAGS) -o $@
	find . -type f -name "*.cpp" -exec touch --no-create {} +

$(BIN_NAME_GUI): $(SOURCES) $(HEADERS) $(GUI_SOURCES) $(GUI_HEADERS)
	@cd $(GUI) && $(QMAKE) $(QFLAGS) && make
	@mv $(shell pwd)/$(GUI)/$(BIN_NAME_GUI) .
	find . -type f -name "*.cpp" -exec touch --no-create {} +

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	-@cd $(GUI) && make clean && rm -f moc_* .qmake.stash
	rm -f $(BIN_NAME_SIM) $(BIN_NAME_GEN) $(BIN_NAME_GUI) $(BIN_NAME_OPT) $(BIN_NAME_PAF) $(GUI)/$(BIN_NAME_GUI) $(GUI)/Makefile $(SRC)/*.o
