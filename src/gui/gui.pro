TEMPLATE          = app
TARGET            = warehouse_manager_gui
QMAKE_CXX         = g++-8
QT               += core gui widgets
QMAKE_CXXFLAGS   += -std=c++17
DEFINES          += QT_DEPRECATED_WARNINGS
DEFINES          += "WHM_GUI" "WHM_SIM" "WHM_OPT" "WHM_GEN"
LIBS             += -lsimlib -lm -lstdc++fs

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
            ../WarehouseOptimizerSLAP.cpp \
            ../WarehouseOptimizerRAND.cpp \
            ../WarehouseSimulatorSIMLIB.cpp \
            ../WarehouseSimulatorCPPSIM.cpp \
            ../WarehouseDataGenerator.cpp \
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
            UiWarehouseOptimizerThread.cpp \
            UiWarehouseGeneratorThread.cpp \
            UiWarehouseSimulatorThread.cpp \
            BaseGraphicItem.cpp \
            BaseShapeGraphicItem.cpp \
            Handle.cpp \
            qcustomplot.cpp

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
            ../WarehouseOptimizerSLAP.h \
            ../WarehouseOptimizerRAND.h \
            ../WarehouseSimulatorSIMLIB.h \
            ../WarehouseSimulatorCPPSIM.h \
            ../WarehouseDataGenerator.h \
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
            UiWarehouseOptimizerThread.h \
            UiWarehouseGeneratorThread.h \
            UiWarehouseSimulatorThread.h \
            BaseGraphicItem.h \
            BaseShapeGraphicItem.h \
            Handle.h \
            qcustomplot.h

RESOURCES += resources.qrc \
             qdarkstyle/style.qrc

FORMS += mainwindow.ui
