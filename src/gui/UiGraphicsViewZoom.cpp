/**
 * Warehouse manager
 *
 * @file    UiGraphicsViewZoom.cpp
 * @date    09/30/2020
 * @brief   This module is used to enable zooming in & out in graphics scene and is based on an Stack overflow answer
 * @author  Pavel Strakhov, answered Oct 1 '13 at 11:02
 * @link    https://stackoverflow.com/a/19114517/8254699
 */

#include <iostream>
#include <qmath.h>
#include <QScrollBar>
#include <QMouseEvent>
#include <QApplication>

#include "UiGraphicsViewZoom.h"

namespace whm
{
    namespace gui
    {
        UiGraphicsViewZoom_t::UiGraphicsViewZoom_t(QGraphicsView* view, QLabel* ratio)
            : QObject(view)
            , view(view)
            , ratio(ratio)
        {
            view->viewport()->installEventFilter(this);
            view->setMouseTracking(true);
            modifiers = Qt::ControlModifier;
            zoomFactorBase = 1.0015;
        }

        void UiGraphicsViewZoom_t::gentleZoom(double factor)
        {
            view->scale(factor, factor);
            view->centerOn(targetScenePos);

            QPointF deltaViewportPos = targetViewportPos - QPointF(view->viewport()->width() / 2.0,
                                                                   view->viewport()->height() / 2.0);
            QPointF viewportCenter = view->mapFromScene(targetScenePos) - deltaViewportPos;

            view->centerOn(view->mapToScene(viewportCenter.toPoint()));
            emit zoomed();
        }

        void UiGraphicsViewZoom_t::setModifiers(Qt::KeyboardModifiers modifiers)
        {
            modifiers = modifiers;
        }

        void UiGraphicsViewZoom_t::setZoomFactorBase(double value)
        {
            zoomFactorBase = value;
        }

        bool UiGraphicsViewZoom_t::eventFilter(QObject *object, QEvent *event)
        {
            if (event->type() == QEvent::MouseMove)
            {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                QPointF delta = targetViewportPos - mouseEvent->pos();

                if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5)
                {
                    targetViewportPos = mouseEvent->pos();
                    targetScenePos = view->mapToScene(mouseEvent->pos());
                }
            }
            else if (event->type() == QEvent::Wheel)
            {
                QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

                if (QApplication::keyboardModifiers() == modifiers)
                {
                    if (wheelEvent->orientation() == Qt::Vertical)
                    {
                        double angle = wheelEvent->angleDelta().y();
                        double factor = qPow(zoomFactorBase, angle);

                        // Update ratio
                        static double actFactor = 1.0;
                        actFactor *= factor;

                        static double meters = ratio->text().toDouble();
                        ratio->setText(QString::number(meters / actFactor));

                        gentleZoom(factor);
                        return true;
                    }
                }
            }

            Q_UNUSED(object)
            return false;
        }
    }
}
