/**
 * Warehouse manager
 *
 * @file    mainwindow.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Entry point of the graphical application
 */

#pragma once

// Qt
#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

namespace whm
{
    namespace gui
    {
        class MainWindow : public QMainWindow
        {
            Q_OBJECT

            public:
                explicit MainWindow(QWidget *parent = Q_NULLPTR);
                virtual ~MainWindow();

                Ui::MainWindow *getUi() const { return ui; }

                QPoint getWidgetPosition(QPoint);

            private slots:
                void mousePressEvent(QMouseEvent *);

                void on_loadLayout_triggered();
                void on_saveLayout_triggered();
                void on_clearLayout_triggered();

                void on_simulationRun_triggered();
                void on_simulationStep_triggered();
                void on_simulationStop_triggered();

                void on_deletionMode_toggled(bool);
                void on_selectionMode_toggled(bool);

                void on_whItemLocations_toggled(bool);
                void on_whItemConveyorRight_toggled(bool);
                void on_whItemConveyorLeft_toggled(bool);
                void on_whItemConveyorUp_toggled(bool);
                void on_whItemConveyorDown_toggled(bool);
                void on_whItemConveyorHub_toggled(bool);

            private:
                Ui::MainWindow *ui;
        };
    }
}
