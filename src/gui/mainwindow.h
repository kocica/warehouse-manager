/**
 * Warehouse manager
 *
 * @file    MainWindow.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Entry point of the graphical application
 */

#pragma once

// Qt
#include <QMainWindow>

namespace whm
{
    namespace gui
    {
        class MainWindow_t : public QMainWindow
        {
            Q_OBJECT

        public:
            explicit MainWindow_t(QWidget *parent = Q_NULLPTR);
            virtual ~MainWindow_t() = default;
        };
    }
}
