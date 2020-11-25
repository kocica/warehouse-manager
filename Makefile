###############################################################
# @file    Makefile                                           #
# @author  Filip Kocica                                       #
# @date    09/30/2020                                         #
#                                                             #
# Warehouse manager makefile                                  #
###############################################################

################## Build constants ##############
BIN_NAME_SIM = warehouse_manager_sim
BIN_NAME_GEN = warehouse_manager_gen
BIN_NAME_GUI = warehouse_manager_gui
BIN_NAME_OPT = warehouse_manager_opt

README     = README

QMAKE      = qmake
QFLAGS     = -qt=qt5

CC         = g++-8
CFLAGS     = -std=c++17 -pedantic -Wall -Wextra -O3 #-fopenmp

PROFILE    = #-pg

LDFLAGS    = -lsimlib -lm -lpython2.7 -l:libC++SIM.a -l:libCommon.a -l:libEvent.a -l:libSimSet.a -l:libStat.a -pthread
LDLIBS     = 
INCLUDE    = -I/usr/include/python2.7

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
	make warehouse_manager_gen
	make warehouse_manager_sim
	make warehouse_manager_opt
	make warehouse_manager_gui

.PHONY: clean

$(BIN_NAME_OPT): CFLAGS += -DWHM_OPT -DWHM_SIM -DWHM_PLOT
$(BIN_NAME_OPT): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(PROFILE) -L$(LDLIBS) -Wl,-rpath=$(LDLIBS) -fPIC $(OBJS) $(LDFLAGS) -o $@
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
#	find . -type f -name "*.cpp" -exec touch --no-create {} +

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	-@cd $(GUI) && make clean && rm -f moc_* .qmake.stash
	rm -f $(BIN_NAME_SIM) $(BIN_NAME_GEN) $(BIN_NAME_GUI) $(BIN_NAME_OPT) $(GUI)/$(BIN_NAME_GUI) $(GUI)/Makefile $(SRC)/*.o
