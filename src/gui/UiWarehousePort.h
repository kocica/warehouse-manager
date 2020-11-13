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

#include "../Logger.h"
#include "mainwindow.h"
#include "UiWarehouseItem.h"
#include "../WarehouseItemType.h"
#include "UiWarehouseConnection.h"

namespace whm
{
    namespace gui
    {
        class UiWarehousePort_t : public BaseShapeGraphicItem_t
        {
            public:
                UiWarehousePort_t(QGraphicsScene*, QGraphicsItem*, MainWindow*, int32_t, int32_t, int32_t, int32_t, int32_t, WarehousePortType_t);
                virtual ~UiWarehousePort_t();

                void select();
                void connect();
                void disconnect();

                void updatePort(double, double, int32_t, int32_t);

                bool isConnected() const;
                void setWhConn(UiWarehouseConnection_t*);
                UiWarehouseConnection_t* getWhConn() const;

                int32_t getWhPortID() const;
                WarehousePortType_t getType() const;
                UiWarehouseItem_t* getWhItem() const;

                void dump() const;

            public slots:
                void mousePressEvent(QGraphicsSceneMouseEvent*);

            private slots:
                void unselect();

            private:
                MainWindow* ui{ nullptr };

                UiWarehouseItem_t* whItem{ nullptr };
                UiWarehouseConnection_t* whConn{ nullptr };

                int32_t whPortID{ 0 };
                WarehousePortType_t whPortType;

                // Shared across all instances
                static UiWarehousePort_t* selectedPort;
        };
    }
}