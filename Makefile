###############################################################
# @file    Makefile                                           #
# @author  Filip Kocica                                       #
# @date    09/30/2020                                         #
#                                                             #
# Warehou manager makefile                                    #
###############################################################

################## Build constants ##############
BIN_NAME   = warehouse_manager

README     = README

QMAKE      = qmake
QFLAGS     = -qt=qt5

CC         = g++
CFLAGS     = -std=c++17

LDFLAGS    =
LIBS       =
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

all: $(BIN_NAME)

$(BIN_NAME): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

debug: CFLAGS += -DDEBUG -g -DIMS_DEBUG
debug: all

gui: $(HEADERS) $(SOURCES) $(OBJS) $(GUI_SOURCES) $(GUI_HEADERS)
	@cd $(GUI) && $(QMAKE) $(QFLAGS) && make
	@mv $(shell pwd)/$(GUI)/$(BIN_NAME)_gui .

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

doxygen:
	$(DOXYGEN) $(CFG)/doxyConf

clean:
	-@cd $(GUI) && make clean && rm -f moc_*
	rm -f $(BIN_NAME) $(BIN_NAME)_gui $(GUI)/$(BIN_NAME)_gui $(GUI)/Makefile $(SRC)/*.o

run:
	./$(BIN_NAME)

rungui:
	./$(BIN_NAME)_gui
