TEMPLATE          = app
TARGET            = warehouse_manager_gui
QT               += core gui widgets
QMAKE_CXXFLAGS   += -std=c++17
DEFINES          += QT_DEPRECATED_WARNINGS
DEFINES          += "WHM_GUI"

OBJECTS += ../tinyxml2.o

SOURCES +=  ../main.cpp \
            ../Utils.cpp \
            ../Logger.cpp \
            ../WarehouseItem.cpp \
            ../WarehousePort.cpp \
            ../WarehouseLayout.cpp \
            ../WarehouseConnection.cpp \
            UiCursor.cpp \
            MainWindow.cpp \
            UiWarehousePort.cpp \
            UiWarehouseItem.cpp \
            UiWarehouseLayout.cpp \
            UiGraphicsViewZoom.cpp \
            UiWarehouseConnection.cpp \
            UiWarehouseItemLocation.cpp \
            UiWarehouseItemConveyor.cpp \
            BaseGraphicItem.cpp \
            BaseShapeGraphicItem.cpp \
            Handle.cpp

HEADERS +=  ../Utils.h \
            ../Logger.h \
            ../WarehouseItem.h \
            ../WarehousePort.h \
            ../WarehouseLayout.h \
            ../WarehouseConnection.h \
            MainWindow.h \
            UiCursor.h \
            UiWarehousePort.h \
            UiWarehouseItem.h \
            UiWarehouseLayout.h \
            UiGraphicsViewZoom.h \
            UiWarehouseConnection.h \
            UiWarehouseItemLocation.h \
            UiWarehouseItemConveyor.h \
            BaseGraphicItem.h \
            BaseShapeGraphicItem.h \
            Handle.h

RESOURCES += resources.qrc

FORMS += mainwindow.ui
