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

namespace whm
{
    namespace gui
    {
        enum class UiWarehouseItemType_t
        {
            E_LOCATION_SHELF,
            E_CONVEYOR
            // E_CONVEYOR_HUB - connector
            // E_CONVEYOR_LTRB - left, top, right, bot
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

        public slots:
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

        protected:
            virtual bool event(QEvent *event);
        };
    }
}