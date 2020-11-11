TEMPLATE          = app
TARGET            = warehouse_manager_gui
QT               += core gui widgets
QMAKE_CXXFLAGS   += -std=c++17
DEFINES          += QT_DEPRECATED_WARNINGS
DEFINES          += "WHM_GUI" "WHM_SIM" "WHM_OPT"
LIBS             += -lsimlib -lm

OBJECTS += ../tinyxml2.o

SOURCES +=  ../main.cpp \
            ../Utils.cpp \
            ../Logger.cpp \
            ../ConfigParser.cpp \
            ../WarehouseItem.cpp \
            ../WarehousePort.cpp \
            ../WarehouseOrder.cpp \
            ../WarehouseLayout.cpp \
            ../WarehouseOrderLine.cpp \
            ../WarehouseConnection.cpp \
            ../WarehousePathFinder.cpp \
            ../WarehouseLocationSlot.cpp \
            ../WarehouseLocationRack.cpp \
            ../WarehouseOptimizerBase.cpp \
            ../WarehouseOptimizerGA.cpp \
            ../WarehouseOptimizerDE.cpp \
            ../WarehouseOptimizerABC.cpp \
            ../WarehouseOptimizerPSO.cpp \
            ../WarehouseSimulator.cpp \
            UiCursor.cpp \
            MainWindow.cpp \
            UiWarehouseSlot.cpp \
            UiWarehousePort.cpp \
            UiWarehouseItem.cpp \
            UiWarehouseLayout.cpp \
            UiGraphicsViewZoom.cpp \
            UiWarehouseItemGate.cpp \
            UiWarehouseConnection.cpp \
            UiWarehouseItemLocation.cpp \
            UiWarehouseItemConveyor.cpp \
            BaseGraphicItem.cpp \
            BaseShapeGraphicItem.cpp \
            Handle.cpp

HEADERS +=  ../Utils.h \
            ../Logger.h \
            ../ConfigParser.h \
            ../WarehouseItem.h \
            ../WarehousePort.h \
            ../WarehouseOrder.h \
            ../WarehouseLayout.h \
            ../WarehouseOrderLine.h \
            ../WarehouseConnection.h \
            ../WarehousePathFinder.h \
            ../WarehouseLocationSlot.h \
            ../WarehouseLocationRack.h \
            ../WarehouseOptimizerBase.h \
            ../WarehouseOptimizerGA.h \
            ../WarehouseOptimizerDE.h \
            ../WarehouseOptimizerABC.h \
            ../WarehouseOptimizerPSO.h \
            ../WarehouseSimulator.h \
            MainWindow.h \
            UiCursor.h \
            UiWarehouseSlot.h \
            UiWarehousePort.h \
            UiWarehouseItem.h \
            UiWarehouseLayout.h \
            UiGraphicsViewZoom.h \
            UiWarehouseItemGate.h \
            UiWarehouseConnection.h \
            UiWarehouseItemLocation.h \
            UiWarehouseItemConveyor.h \
            BaseGraphicItem.h \
            BaseShapeGraphicItem.h \
            Handle.h

RESOURCES += resources.qrc

FORMS += mainwindow.ui
