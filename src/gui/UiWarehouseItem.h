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
//#include "UiWarehousePort.h"

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

        class UiWarehouseItem_t : public QPushButton
        {
            Q_OBJECT

        public:
            UiWarehouseItem_t(QWidget *, MainWindow*, QPoint, UiWarehouseItemType_t);
            virtual ~UiWarehouseItem_t();

            QPoint pos() const;
            int32_t getWhItemID() const;
            UiWarehouseItemType_t getWhItemType() const;

            void dump() const;

        public slots:
            void eraseFromLayout();
            void mouseMoveEvent(QMouseEvent *);
            void mousePressEvent(QMouseEvent *);
            bool gestureEvent(QGestureEvent *event);

        protected:
            MainWindow *ui{ nullptr };

            bool holdTriggered{ false };

            int32_t sizeX{ 0 };
            int32_t sizeY{ 0 };

            int32_t whItemID{ 0 };
            UiWarehouseItemType_t whItemType;

            virtual bool event(QEvent *event);

            std::vector<UiWarehousePort_t*> ports;
        };
    }
}