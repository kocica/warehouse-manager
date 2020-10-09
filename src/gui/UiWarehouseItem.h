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

#include "mainwindow.h"
#include "BaseShapeGraphicItem.h"
#include "../WarehouseItemType.h"

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

                int32_t getWhItemID() const;
                WarehouseItemType_t getWhItemType() const;

                void dump() const;

                int32_t getID() const { return whItemID; }
                int32_t getX()  const { return mRect.x(); }
                int32_t getY()  const { return mRect.y(); }
                int32_t getW()  const { return mRect.width(); }
                int32_t getH()  const { return mRect.height(); }

                void setID(int32_t id) { whItemID = id; }
                void setX(int32_t x)   { mRect.setX(x); }
                void setY(int32_t y)   { mRect.setY(y); }
                void setW(int32_t w)   { mRect.setWidth(w); }
                void setH(int32_t h)   { mRect.setHeight(h); }

                UiWarehousePortContainer_t getWhPorts() const;

            protected:
                void eraseFromLayout();
                void mousePressEvent(QGraphicsSceneMouseEvent *);

            protected:
                MainWindow* ui{ nullptr };
                QGraphicsScene* scene{ nullptr };

                int32_t whItemID{ 0 };
                WarehouseItemType_t whItemType;

                UiWarehousePortContainer_t whPorts;
        };
    }
}