/**
 * Warehouse manager
 *
 * @file    UiGraphicsViewZoom.h
 * @date    09/30/2020
 * @brief   This module is used to enable zooming in & out in graphics scene and is based on an Stack overflow answer
 * @author  Pavel Strakhov, answered Oct 1 '13 at 11:02
 * @link    https://stackoverflow.com/a/19114517/8254699
 */

#pragma once

#include <QLabel>
#include <QObject>
#include <QGraphicsView>

namespace whm
{
    namespace gui
    {
        class UiGraphicsViewZoom_t : public QObject
        {
            Q_OBJECT
        
            public:
                UiGraphicsViewZoom_t(QGraphicsView* view, QLabel* ratio);
                virtual ~UiGraphicsViewZoom_t() = default;

                void gentleZoom(double factor);
                void setModifiers(Qt::KeyboardModifiers modifiers);
                void setZoomFactorBase(double value);

            private:
                QGraphicsView* view;
                QLabel* ratio;

                Qt::KeyboardModifiers modifiers;

                double zoomFactorBase{ 0.0 };

                QPointF targetScenePos;
                QPointF targetViewportPos;

                bool eventFilter(QObject* object, QEvent* event);

            signals:
                void zoomed();
        };
    }
}
