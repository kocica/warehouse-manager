/**
 * Warehouse manager
 *
 * @file    UiGraphicsViewZoom.h
 * @date    09/30/2020
 * @author  TODO: Citation
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
