/**
 * Warehouse manager
 *
 * @file    UiWarehousePort.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse port
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>

#include "mainwindow.h"
#include "UiWarehouseItem.h"

namespace whm
{
    namespace gui
    {
        class UiWarehousePort_t : QPushButton
        {
            Q_OBJECT

            public:
                UiWarehousePort_t(QWidget *parent, MainWindow *ui, UiWarehouseItem_t *whItem, int32_t x, int32_t y);
                virtual ~UiWarehousePort_t() = default;

                void select();

                UiWarehouseItem_t* getWhItem() const;

            public slots:
                void mousePressEvent(QMouseEvent *);

            private slots:
                void unselect();

            private:
                MainWindow *ui{ nullptr };
                UiWarehouseItem_t* whItem{ nullptr };
                static UiWarehousePort_t *selectedPort;
        };
    }
}