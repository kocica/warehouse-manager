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

README     = README

QMAKE      = qmake
QFLAGS     = -qt=qt5

CC         = g++
CFLAGS     = -std=c++17 -pedantic -Wall -Wextra

LDFLAGS    = libs/simlib.so
LDLIBS     = 
DOXYGEN    = doxygen

SRC        = src
CFG        = cfg
GUI        = $(SRC)/gui

HEADERS    = $(wildcard $(SRC)/*.h)
SOURCES    = $(wildcard $(SRC)/*.cpp)
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))

GUI_SOURCES = $(wildcard $(GUI)/*.cpp)
GUI_HEADERS = $(wildcard $(GUI)/*.h)
GUI_OBJS    = $(patsubst %.cpp, %.o, $(GUI_SOURCES))

#all: $(BIN_NAME_SIM) $(BIN_NAME_GEN) $(BIN_NAME_GUI)
.PHONY: clean

$(BIN_NAME_GEN): CFLAGS += -DWHM_GEN
$(BIN_NAME_GEN): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) -L$(LDLIBS) -Wl,-rpath=$(LDLIBS) -fPIC $(OBJS) -o $@ $(LDFLAGS)

$(BIN_NAME_SIM): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) -L$(LDLIBS) -Wl,-rpath=$(LDLIBS) -fPIC $(OBJS) -o $@ $(LDFLAGS)

$(BIN_NAME_GUI): $(HEADERS) $(SOURCES) $(OBJS) $(GUI_SOURCES) $(GUI_HEADERS)
	@cd $(GUI) && $(QMAKE) $(QFLAGS) && make
	@mv $(shell pwd)/$(GUI)/$(BIN_NAME_GUI) .

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

doxygen:
	$(DOXYGEN) $(CFG)/doxyConf

clean:
	-@cd $(GUI) && make clean && rm -f moc_* .qmake.stash
	rm -f $(BIN_NAME_SIM) $(BIN_NAME_GEN) $(BIN_NAME_GUI) $(GUI)/$(BIN_NAME_GUI) $(GUI)/Makefile $(SRC)/*.o
