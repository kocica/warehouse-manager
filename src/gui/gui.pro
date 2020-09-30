TEMPLATE          = app
TARGET            = warehouse_manager_gui
QT               += core gui widgets
QMAKE_CXXFLAGS   += -std=c++17
DEFINES          += QT_DEPRECATED_WARNINGS
DEFINES          += "WHM_GUI"

OBJECTS += ../Utils.o

SOURCES +=  ../main.cpp \
            MainWindow.cpp

HEADERS +=  MainWindow.h

#
# RESOURCES += resources.qrc
#
