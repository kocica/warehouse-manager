/**
 * Warehouse manager
 *
 * @file    UiWarehouseItem.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse item (location, conv, ...)
 */

#include <iostream>

#include <QMessageBox>

#include "UiCursor.h"
#include "UiWarehouseItem.h"
#include "UiWarehousePort.h"
#include "UiWarehouseLayout.h"

namespace
{
    
}

namespace whm
{
    namespace gui
    {
        std::map<UiWarehouseItemType_t, std::vector<UiWarehouseItemType_t>> allowedCombinations =
        {
            { UiWarehouseItemType_t::E_CONVEYOR_R,     { UiWarehouseItemType_t::E_LOCATION_SHELF, UiWarehouseItemType_t::E_CONVEYOR_HUB }},
            { UiWarehouseItemType_t::E_CONVEYOR_L,     { UiWarehouseItemType_t::E_LOCATION_SHELF, UiWarehouseItemType_t::E_CONVEYOR_HUB }},
            { UiWarehouseItemType_t::E_CONVEYOR_U,     { UiWarehouseItemType_t::E_LOCATION_SHELF, UiWarehouseItemType_t::E_CONVEYOR_HUB }},
            { UiWarehouseItemType_t::E_CONVEYOR_D,     { UiWarehouseItemType_t::E_LOCATION_SHELF, UiWarehouseItemType_t::E_CONVEYOR_HUB }},
            { UiWarehouseItemType_t::E_CONVEYOR_HUB,   { UiWarehouseItemType_t::E_CONVEYOR_R,
                                                         UiWarehouseItemType_t::E_CONVEYOR_L,
                                                         UiWarehouseItemType_t::E_CONVEYOR_U,
                                                         UiWarehouseItemType_t::E_CONVEYOR_D }},
            { UiWarehouseItemType_t::E_LOCATION_SHELF, { UiWarehouseItemType_t::E_CONVEYOR_R,
                                                         UiWarehouseItemType_t::E_CONVEYOR_L,
                                                         UiWarehouseItemType_t::E_CONVEYOR_U,
                                                         UiWarehouseItemType_t::E_CONVEYOR_D }},
        };

        bool isWhItemCombinationAllowed(UiWarehouseItemType_t lhs, UiWarehouseItemType_t rhs)
        {
            std::vector<UiWarehouseItemType_t>& allowedTypes = allowedCombinations[lhs];

            return std::any_of(allowedTypes.begin(), allowedTypes.end(),
                               [&](UiWarehouseItemType_t& whItemType) -> bool
                               {
                                   return whItemType == rhs;
                               });
        }

        UiWarehouseItem_t::UiWarehouseItem_t(QWidget *p, MainWindow* ui, QPoint loc, UiWarehouseItemType_t type)
            : QPushButton(p)
            , ui(ui)
            , whItemID(UiWarehouseLayout_t::getWhLayout().getNextWhItemID())
            , whItemType(type)
        {
            (void) loc;

            grabGesture(Qt::TapAndHoldGesture);
            QTapAndHoldGesture::setTimeout(100);
        }

        UiWarehouseItem_t::~UiWarehouseItem_t()
        {
            for (UiWarehousePort_t* port : ports)
            {
                delete port;
            }

            ports.clear();
        }

        bool UiWarehouseItem_t::event(QEvent *event)
        {
            if (event->type() == QEvent::Gesture)
            {
                return gestureEvent(static_cast<QGestureEvent*>(event));
            }

            return QWidget::event(event);
        }

        bool UiWarehouseItem_t::gestureEvent(QGestureEvent *event)
        {
            if (event->gesture(Qt::TapAndHoldGesture))
            {
                holdTriggered = true;
            }

            return true;
        }

        QPoint UiWarehouseItem_t::pos() const
        {
            QPoint p = QWidget::pos();

            p.rx() += sizeX/2;
            p.ry() += sizeY/2;

            return p;
        }

        int32_t UiWarehouseItem_t::getWhItemID() const
        {
            return whItemID;
        }

        UiWarehouseItemType_t UiWarehouseItem_t::getWhItemType() const
        {
            return whItemType;
        }

        void UiWarehouseItem_t::eraseFromLayout()
        {
            UiWarehouseLayout_t::getWhLayout().eraseWhItem(this);
        }

        void UiWarehouseItem_t::mousePressEvent(QMouseEvent *event)
        {
            if (event->button() == Qt::LeftButton)
            {
                holdTriggered = false;

                if(UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    eraseFromLayout();
                    delete this;
                }
            }
            else if (event->button() == Qt::RightButton)
            {
                QMenu contextMenu("Item manipulation", this);
                contextMenu.setStyleSheet("background-color: rgb(150,150,150);");

                QAction actionDelete("Delete", this);
                connect(&actionDelete, SIGNAL(triggered()), this, SLOT(deleteLater()));
                connect(&actionDelete, SIGNAL(triggered()), this, SLOT(eraseFromLayout()));
                contextMenu.addAction(&actionDelete);
                contextMenu.exec(mapToGlobal(event->pos()));

                // TODO: Resize, etc.
            }
        }

        void UiWarehouseItem_t::mouseMoveEvent(QMouseEvent *event)
        {
            (void) event;

            if (holdTriggered)
            {
                if(std::any_of(ports.begin(), ports.end(),
                               [](UiWarehousePort_t* p) -> bool
                               {
                                   return p->isConnected();
                               }))
                {
                    std::cout << "Cannot move, first disconnect!" << std::endl;
                    return;
                }

                QPoint mousePos = QCursor::pos();
                mousePos = parentWidget()->mapFromGlobal(mousePos);

                if (mousePos.x() < sizeX/2)
                {
                    mousePos.rx() = sizeX/2;
                }

                if (mousePos.y() < sizeY/2)
                {
                    mousePos.ry() = sizeY/2;
                }

                if (mousePos.x() > parentWidget()->minimumWidth() - sizeX/2)
                {
                    parentWidget()->setMinimumWidth(mousePos.x() + sizeX/2);
                }

                if (mousePos.y() > parentWidget()->minimumHeight() - sizeY/2)
                {
                    parentWidget()->setMinimumHeight(mousePos.y() + sizeY/2);
                }

                mousePos.rx() -= sizeX/2;
                mousePos.ry() -= sizeY/2;

                mousePos.rx() -= mousePos.rx() % 20 - 1;
                mousePos.ry() -= mousePos.ry() % 20 - 1;

                // Don't move in case it collides with any other item
                if (UiWarehouseLayout_t::getWhLayout().itemsIntersects(this))
                {
                    //QMessageBox err;
                    //err.critical(0, "Collision detected", "Item's cannot intersect.");
                    //err.setFixedSize(500,200);

                    std::cout << "Collision detected - Item's cannot intersect." << std::endl;
                }
                else
                {
                    this->move(mousePos);
                }
            }
        }

        void UiWarehouseItem_t::dump() const
        {
            std::cout << "==============================================" << std::endl;
            std::cout << "  Warehouse item ID <" << whItemID << "> Type <" << typeid(*this).name() << ">" << std::endl;
            std::cout << "==============================================" << std::endl;
 
            std::for_each(ports.begin(), ports.end(),
                          [](UiWarehousePort_t* p)
                          {
                              p->dump();
                          });

            std::cout << "==============================================" << std::endl;
        }
    }
}