TEMPLATE          = app
TARGET            = warehouse_manager_gui
QT               += core gui widgets
QMAKE_CXXFLAGS   += -std=c++17
DEFINES          += QT_DEPRECATED_WARNINGS
DEFINES          += "WHM_GUI"

OBJECTS += ../Utils.o

SOURCES +=  ../main.cpp \
            UiCursor.cpp \
            MainWindow.cpp \
            UiWarehousePort.cpp \
            UiWarehouseItem.cpp \
            UiWarehouseLayout.cpp \
            UiGraphicsViewZoom.cpp \
            UiWarehouseConnection.cpp \
            UiWarehouseItemLocation.cpp \
            UiWarehouseItemConveyor.cpp

HEADERS +=  MainWindow.h \
            UiCursor.h \
            UiWarehousePort.h \
            UiWarehouseItem.h \
            UiWarehouseLayout.h \
            UiGraphicsViewZoom.h \
            UiWarehouseConnection.h \
            UiWarehouseItemLocation.h \
            UiWarehouseItemConveyor.h

RESOURCES += resources.qrc

FORMS += mainwindow.ui
