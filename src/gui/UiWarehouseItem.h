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

namespace whm
{
    namespace gui
    {
        // Forward declaration due to circular include
        class UiWarehousePort_t;

        enum class UiWarehouseItemType_t
        {
            E_LOCATION_SHELF,
            E_CONVEYOR_R,  //< Convs to all directions, so we can detect flow
            E_CONVEYOR_L,
            E_CONVEYOR_U,
            E_CONVEYOR_D,
            E_CONVEYOR_HUB //< Able to connect up to four convs
        };

        bool isWhItemCombinationAllowed(UiWarehouseItemType_t, UiWarehouseItemType_t);

        class UiWarehouseItem_t : public BaseShapeGraphicItem_t
        {
            using UiWarehousePortContainer_t = std::vector<UiWarehousePort_t*>;

            public:
                UiWarehouseItem_t(QGraphicsScene*, MainWindow*, int32_t, int32_t, int32_t, int32_t, UiWarehouseItemType_t);
                virtual ~UiWarehouseItem_t();

                int32_t getWhItemID() const;
                UiWarehouseItemType_t getWhItemType() const;

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
                UiWarehouseItemType_t whItemType;

                UiWarehousePortContainer_t whPorts;
        };
    }
}