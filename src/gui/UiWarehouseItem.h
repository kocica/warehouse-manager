/**
 * Warehouse manager
 *
 * @file    UiWarehouseItem.h
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse item (location, conv, ...)
 */

#pragma once

#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QMouseEvent>
#include <QPushButton>
#include <QTapAndHoldGesture>

#include "../Logger.h"
#include "mainwindow.h"
#include "BaseShapeGraphicItem.h"
#include "../WarehouseTypes.h"

namespace whm
{
    namespace gui
    {
        // Forward declaration due to circular include
        class UiWarehousePort_t;

        bool isWhItemCombinationAllowed(WarehouseItemType_t, WarehouseItemType_t);

        class UiWarehouseItem_t : public BaseShapeGraphicItem_t
        {
            using UiWarehousePortContainer_t = std::vector<UiWarehousePort_t*>;

            public:
                UiWarehouseItem_t(QGraphicsScene*, MainWindow*, int32_t, int32_t, int32_t, int32_t, WarehouseItemType_t);
                virtual ~UiWarehouseItem_t();

                void dump() const;
                void disconnect();
                void removeWhItem();
                bool isConnected() const;
                void setItemHeat(double);

                int32_t getWhItemID() const { return whItemID; }
                int32_t getX() const { return this->scenePos().x() + this->mRect.topLeft().x(); }
                int32_t getY() const { return this->scenePos().y() + this->mRect.topLeft().y(); }
                int32_t getW() const { return mRect.width(); }
                int32_t getH() const { return mRect.height(); }
                int32_t getO() const { return this->mOrientation; }

                void setWhItemID(int32_t id) { whItemID = id; }
                void setX(int32_t x)   { mRect.setX(x); }
                void setY(int32_t y)   { mRect.setY(y); }
                void setW(int32_t w)   { mRect.setWidth(w); }
                void setH(int32_t h)   { mRect.setHeight(h); }
                void setO(int32_t o)   { this->setGraphicItemOrientation(o); }

                void moveItem(qreal, qreal);

                MainWindow* getUi() const;
                WarehouseItemType_t getWhItemType() const;
                UiWarehousePortContainer_t getWhPorts() const;

            protected:
                void eraseFromLayout();
                void mousePressEvent(QGraphicsSceneMouseEvent*);
                void hoverEnterEvent(QGraphicsSceneHoverEvent*);
                void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

            protected:
                MainWindow* ui{ nullptr };
                QGraphicsScene* scene{ nullptr };
                QGraphicsRectItem* heatRect{ nullptr };

                int32_t portSizeX{ 0 };
                int32_t portSizeY{ 0 };

                double workload{ 0.0 };

                int32_t whItemID{ 0 };
                WarehouseItemType_t whItemType;

                UiWarehousePortContainer_t whPorts;
        };
    }
}