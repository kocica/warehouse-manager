/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *saveLayout;
    QAction *loadLayout;
    QAction *simulationRun;
    QAction *simulationStep;
    QAction *simulationStop;
    QAction *clearLayout;
    QAction *actionLabel;
    QAction *labelItems;
    QActionGroup *type_selection;
    QAction *actionInt_selected;
    QAction *actionFloat_seleted;
    QAction *actionHex_selected;
    QActionGroup *block_selection;
    QAction *actionSelect_pow;
    QAction *actionSelect_mul;
    QAction *actionSelect_div;
    QAction *deletionMode;
    QAction *selectionMode;
    QAction *whItemLocations;
    QAction *whItemConveyorRight;
    QAction *whItemConveyorLeft;
    QAction *whItemConveyorUp;
    QAction *whItemConveyorDown;
    QAction *whItemConveyorHub;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *warehouseLayoutArea;
    QGridLayout *gridLayout_2;
    QVBoxLayout *frameLayout;
    QFrame *frame;
    QGraphicsView *view;
    QToolBar *mainToolBar;
    QToolBar *layoutManagement;
    QToolBar *simulationManagement;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1130, 901);
        saveLayout = new QAction(MainWindow);
        saveLayout->setObjectName(QStringLiteral("saveLayout"));
        loadLayout = new QAction(MainWindow);
        loadLayout->setObjectName(QStringLiteral("loadLayout"));
        simulationRun = new QAction(MainWindow);
        simulationRun->setObjectName(QStringLiteral("simulationRun"));
        simulationStep = new QAction(MainWindow);
        simulationStep->setObjectName(QStringLiteral("simulationStep"));
        simulationStop = new QAction(MainWindow);
        simulationStop->setObjectName(QStringLiteral("simulationStop"));
        clearLayout = new QAction(MainWindow);
        clearLayout->setObjectName(QStringLiteral("clearLayout"));
        actionLabel = new QAction(MainWindow);
        actionLabel->setObjectName(QStringLiteral("actionLabel"));
        actionLabel->setEnabled(false);
        labelItems = new QAction(MainWindow);
        labelItems->setObjectName(QStringLiteral("labelItems"));
        labelItems->setEnabled(false);
        type_selection = new QActionGroup(MainWindow);
        type_selection->setObjectName(QStringLiteral("type_selection"));
        actionInt_selected = new QAction(type_selection);
        actionInt_selected->setObjectName(QStringLiteral("actionInt_selected"));
        actionInt_selected->setCheckable(true);
        actionFloat_seleted = new QAction(type_selection);
        actionFloat_seleted->setObjectName(QStringLiteral("actionFloat_seleted"));
        actionFloat_seleted->setCheckable(true);
        actionHex_selected = new QAction(type_selection);
        actionHex_selected->setObjectName(QStringLiteral("actionHex_selected"));
        actionHex_selected->setCheckable(true);
        block_selection = new QActionGroup(MainWindow);
        block_selection->setObjectName(QStringLiteral("block_selection"));
        actionSelect_pow = new QAction(block_selection);
        actionSelect_pow->setObjectName(QStringLiteral("actionSelect_pow"));
        actionSelect_pow->setCheckable(true);
        actionSelect_mul = new QAction(block_selection);
        actionSelect_mul->setObjectName(QStringLiteral("actionSelect_mul"));
        actionSelect_mul->setCheckable(true);
        actionSelect_div = new QAction(block_selection);
        actionSelect_div->setObjectName(QStringLiteral("actionSelect_div"));
        actionSelect_div->setCheckable(true);
        deletionMode = new QAction(block_selection);
        deletionMode->setObjectName(QStringLiteral("deletionMode"));
        deletionMode->setCheckable(true);
        QIcon icon;
        icon.addFile(QStringLiteral("../../icons/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        deletionMode->setIcon(icon);
        selectionMode = new QAction(block_selection);
        selectionMode->setObjectName(QStringLiteral("selectionMode"));
        selectionMode->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral("../../cursor.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectionMode->setIcon(icon1);
        whItemLocations = new QAction(block_selection);
        whItemLocations->setObjectName(QStringLiteral("whItemLocations"));
        whItemLocations->setCheckable(true);
        whItemConveyorRight = new QAction(block_selection);
        whItemConveyorRight->setObjectName(QStringLiteral("whItemConveyorRight"));
        whItemConveyorRight->setCheckable(true);
        whItemConveyorLeft = new QAction(block_selection);
        whItemConveyorLeft->setObjectName(QStringLiteral("whItemConveyorLeft"));
        whItemConveyorLeft->setCheckable(true);
        whItemConveyorUp = new QAction(block_selection);
        whItemConveyorUp->setObjectName(QStringLiteral("whItemConveyorUp"));
        whItemConveyorUp->setCheckable(true);
        whItemConveyorDown = new QAction(block_selection);
        whItemConveyorDown->setObjectName(QStringLiteral("whItemConveyorDown"));
        whItemConveyorDown->setCheckable(true);
        whItemConveyorHub = new QAction(block_selection);
        whItemConveyorHub->setObjectName(QStringLiteral("whItemConveyorHub"));
        whItemConveyorHub->setCheckable(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setStyleSheet(QLatin1String("background-color: rgb(200,200,200);\n"
"vertical {background-color: rgb(0,0,0);}"));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        warehouseLayoutArea = new QWidget();
        warehouseLayoutArea->setObjectName(QStringLiteral("warehouseLayoutArea"));
        warehouseLayoutArea->setGeometry(QRect(0, 0, 1041, 872));
        gridLayout_2 = new QGridLayout(warehouseLayoutArea);
        gridLayout_2->setSpacing(1);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(1, 1, 1, 1);
        frameLayout = new QVBoxLayout();
        frameLayout->setSpacing(0);
        frameLayout->setObjectName(QStringLiteral("frameLayout"));
        frame = new QFrame(warehouseLayoutArea);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setMinimumSize(QSize(0, 0));
        frame->setLayoutDirection(Qt::LeftToRight);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        view = new QGraphicsView(frame);
        view->setObjectName(QStringLiteral("view"));
        view->setGeometry(QRect(0, 0, 1041, 871));

        frameLayout->addWidget(frame);


        gridLayout_2->addLayout(frameLayout, 0, 0, 1, 1);

        scrollArea->setWidget(warehouseLayoutArea);

        verticalLayout->addWidget(scrollArea);


        verticalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setMovable(true);
        mainToolBar->setAllowedAreas(Qt::AllToolBarAreas);
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
        mainToolBar->setFloatable(true);
        MainWindow->addToolBar(Qt::LeftToolBarArea, mainToolBar);
        layoutManagement = new QToolBar(MainWindow);
        layoutManagement->setObjectName(QStringLiteral("layoutManagement"));
        layoutManagement->setLayoutDirection(Qt::LeftToRight);
        layoutManagement->setMovable(false);
        layoutManagement->setToolButtonStyle(Qt::ToolButtonTextOnly);
        MainWindow->addToolBar(Qt::TopToolBarArea, layoutManagement);
        simulationManagement = new QToolBar(MainWindow);
        simulationManagement->setObjectName(QStringLiteral("simulationManagement"));
        simulationManagement->setLayoutDirection(Qt::LeftToRight);
        simulationManagement->setMovable(true);
        simulationManagement->setFloatable(true);
        MainWindow->addToolBar(Qt::TopToolBarArea, simulationManagement);

        mainToolBar->addAction(selectionMode);
        mainToolBar->addAction(deletionMode);
        mainToolBar->addSeparator();
        mainToolBar->addAction(labelItems);
        mainToolBar->addSeparator();
        mainToolBar->addAction(whItemLocations);
        mainToolBar->addAction(whItemConveyorRight);
        mainToolBar->addAction(whItemConveyorLeft);
        mainToolBar->addAction(whItemConveyorUp);
        mainToolBar->addAction(whItemConveyorDown);
        mainToolBar->addAction(whItemConveyorHub);
        mainToolBar->addSeparator();
        layoutManagement->addAction(loadLayout);
        layoutManagement->addAction(saveLayout);
        layoutManagement->addAction(clearLayout);
        simulationManagement->addAction(simulationRun);
        simulationManagement->addAction(simulationStep);
        simulationManagement->addAction(simulationStop);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        saveLayout->setText(QApplication::translate("MainWindow", "Save", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        saveLayout->setToolTip(QApplication::translate("MainWindow", "Save the current layout into a file", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        loadLayout->setText(QApplication::translate("MainWindow", "Load", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        loadLayout->setToolTip(QApplication::translate("MainWindow", "Load a layout from a file", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        simulationRun->setText(QApplication::translate("MainWindow", "Run", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        simulationRun->setToolTip(QApplication::translate("MainWindow", "Compute all at once", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        simulationStep->setText(QApplication::translate("MainWindow", "Step", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        simulationStep->setToolTip(QApplication::translate("MainWindow", "Compute one block", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        simulationStop->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        simulationStop->setToolTip(QApplication::translate("MainWindow", "Stop computation", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        clearLayout->setText(QApplication::translate("MainWindow", "Clear", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        clearLayout->setToolTip(QApplication::translate("MainWindow", "Erase everything", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionLabel->setText(QApplication::translate("MainWindow", "Value", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionLabel->setToolTip(QApplication::translate("MainWindow", "Select a block value type below", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        labelItems->setText(QApplication::translate("MainWindow", "Items", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        labelItems->setToolTip(QApplication::translate("MainWindow", "Select location item to be used", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionInt_selected->setText(QApplication::translate("MainWindow", "Int", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionInt_selected->setToolTip(QApplication::translate("MainWindow", "Blocks using integer (whole number) operations", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionFloat_seleted->setText(QApplication::translate("MainWindow", "Float", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionFloat_seleted->setToolTip(QApplication::translate("MainWindow", "Blocks using floating point operations", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionHex_selected->setText(QApplication::translate("MainWindow", "Hex", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionHex_selected->setToolTip(QApplication::translate("MainWindow", "Blocks using hexadecimal notation", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionSelect_pow->setText(QApplication::translate("MainWindow", "Pow", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionSelect_pow->setToolTip(QApplication::translate("MainWindow", "Place blocks: Top to the power of Bot", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionSelect_pow->setShortcut(QApplication::translate("MainWindow", "5", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionSelect_mul->setText(QApplication::translate("MainWindow", "Mul", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionSelect_mul->setToolTip(QApplication::translate("MainWindow", "Place blocks: multiplication", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionSelect_mul->setShortcut(QApplication::translate("MainWindow", "3", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        actionSelect_div->setText(QApplication::translate("MainWindow", "Div", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionSelect_div->setToolTip(QApplication::translate("MainWindow", "Place blocks: Top divided by Bot", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionSelect_div->setShortcut(QApplication::translate("MainWindow", "4", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        deletionMode->setText(QApplication::translate("MainWindow", "Delete", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        deletionMode->setToolTip(QApplication::translate("MainWindow", "Switch to deleting mode", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        selectionMode->setText(QApplication::translate("MainWindow", "Select", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        selectionMode->setToolTip(QApplication::translate("MainWindow", "Switch to default cursor", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        selectionMode->setShortcut(QApplication::translate("MainWindow", "Esc", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        whItemLocations->setText(QApplication::translate("MainWindow", "Location shelf", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        whItemLocations->setToolTip(QApplication::translate("MainWindow", "Warehouse location shelf", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        whItemLocations->setShortcut(QApplication::translate("MainWindow", "1", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        whItemConveyorRight->setText(QApplication::translate("MainWindow", "Conveyor right", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        whItemConveyorRight->setToolTip(QApplication::translate("MainWindow", "Warehouse conveyor system", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        whItemConveyorRight->setShortcut(QApplication::translate("MainWindow", "2", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        whItemConveyorLeft->setText(QApplication::translate("MainWindow", "Conveyor left", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        whItemConveyorLeft->setToolTip(QApplication::translate("MainWindow", "Warehouse conveyor system", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        whItemConveyorLeft->setShortcut(QApplication::translate("MainWindow", "2", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        whItemConveyorUp->setText(QApplication::translate("MainWindow", "Conveyor up", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        whItemConveyorUp->setToolTip(QApplication::translate("MainWindow", "Warehouse conveyor system", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        whItemConveyorUp->setShortcut(QApplication::translate("MainWindow", "2", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        whItemConveyorDown->setText(QApplication::translate("MainWindow", "Conveyor down", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        whItemConveyorDown->setToolTip(QApplication::translate("MainWindow", "Warehouse conveyor system", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        whItemConveyorDown->setShortcut(QApplication::translate("MainWindow", "2", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        whItemConveyorHub->setText(QApplication::translate("MainWindow", "Conveyor hub", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        whItemConveyorHub->setToolTip(QApplication::translate("MainWindow", "Warehouse conveyor system", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        whItemConveyorHub->setShortcut(QApplication::translate("MainWindow", "2", Q_NULLPTR));
#endif // QT_NO_SHORTCUT
        layoutManagement->setWindowTitle(QApplication::translate("MainWindow", "toolBar", Q_NULLPTR));
        simulationManagement->setWindowTitle(QApplication::translate("MainWindow", "toolBar_2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
