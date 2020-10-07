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
#include "UiWarehouseConnection.h"

namespace whm
{
    namespace gui
    {
        class UiWarehousePort_t : BaseShapeGraphicItem_t
        {
            public:
                UiWarehousePort_t(QGraphicsScene *parent, QGraphicsItem* item, MainWindow *ui, UiWarehouseItem_t *whItem, int32_t x, int32_t y);
                virtual ~UiWarehousePort_t();

                void select();
                void connect();
                void disconnect();

                bool isConnected() const;
                void setWhConn(UiWarehouseConnection_t*);

                UiWarehouseItem_t* getWhItem() const;

                void dump() const;

            public slots:
                void mousePressEvent(QGraphicsSceneMouseEvent *);

            private slots:
                void unselect();

            private:
                MainWindow *ui{ nullptr };

                UiWarehouseItem_t* whItem{ nullptr };
                UiWarehouseConnection_t* whConn{ nullptr };

                // Shared across all instances
                static UiWarehousePort_t *selectedPort;
        };
    }
}