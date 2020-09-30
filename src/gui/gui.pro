TEMPLATE          = app
TARGET            = warehouse_manager_gui
QT               += core gui widgets
QMAKE_CXXFLAGS   += -std=c++17
DEFINES          += QT_DEPRECATED_WARNINGS
DEFINES          += "GUI"

OBJECTS += ../Grid.o \
           ../Cell.o \
           ../TAV.o \
           ../Args.o

SOURCES +=  ../main.cpp \
            mainwindow.cpp \
            GridUI.cpp \
            RightClickButton.cpp \
            ImageWidget.cpp

HEADERS +=  mainwindow.h \
            GridUI.h \
            RightClickButton.h \
            ImageWidget.h

RESOURCES += resources.qrc
