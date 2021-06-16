/**
 * Warehouse manager
 *
 * @file    UiWarehousePort.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse port
 */

#include <iostream>

#include <QMessageBox>

#include "UiCursor.h"
#include "UiWarehousePort.h"
#include "UiWarehouseItem.h"
#include "UiWarehouseLayout.h"
#include "UiWarehouseConnection.h"

namespace
{
    using whm::WarehousePortType_t;

    bool isPortCombinationAllowed(WarehousePortType_t lhs, WarehousePortType_t rhs)
    {
        return !((lhs == WarehousePortType_t::E_PORT_LEFT  && rhs == WarehousePortType_t::E_PORT_LEFT) ||
                 (lhs == WarehousePortType_t::E_PORT_RIGHT && rhs == WarehousePortType_t::E_PORT_RIGHT));
    }
}

namespace whm
{
    namespace gui
    {
        UiWarehousePort_t *UiWarehousePort_t::selectedPort = nullptr;

        UiWarehousePort_t::UiWarehousePort_t(QGraphicsScene *s, QGraphicsItem* parent, MainWindow *ui, int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, WarehousePortType_t t)
            : BaseShapeGraphicItem_t(x, y, w, h, BaseShapeGraphicItem_t::ITEM_PIXMAP, ui, s, parent)
            , ui(ui)
            , whItem(dynamic_cast<UiWarehouseItem_t*>(parent))
            , whPortID(id)
            , whPortType(t)
        {
            this->showHandles(false);

            unmark();
        }

        UiWarehousePort_t::~UiWarehousePort_t()
        {
            if (this->isConnected())
            {
                delete whConn;
            }

            if(selectedPort == this)
            {
                selectedPort = nullptr;
            }
        }

        void UiWarehousePort_t::mousePressEvent(QGraphicsSceneMouseEvent* event)
        {
            if (this->isConnected())
            {
                whItem->mousePressEvent(event);
                return;
            }

            if (event->button() == Qt::LeftButton)
            {
                if (UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    whItem->removeWhItem();
                    return;
                }
                else
                {
                    if (selectedPort == this)
                    {
                        unselect();
                    }
                    else if (selectedPort == nullptr)
                    {
                        if(!this->isConnected())
                        {
                            select();
                        }
                    }
                    else if (selectedPort->getWhItem() == this->getWhItem())
                    {
                        QMessageBox::critical(nullptr, "Error", "Cannot connect same object!");
                        Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot connect same object!");
                    }
                    else
                    {
                        if (!isPortCombinationAllowed(this->getType(),
                                              selectedPort->getType()))
                        {
                            QMessageBox::critical(nullptr, "Error", "Cannot connect incompatible ports!");
                            Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot connect incompatible ports!");
                            return;
                        }

                        if (isWhItemCombinationAllowed(this->getWhItem()->getWhItemType(),
                                               selectedPort->getWhItem()->getWhItemType()))
                        {
                            auto dstItem = this->getWhItem();
                            auto dstPort = this;
                            auto srcItem = selectedPort->getWhItem();
                            auto srcPort = selectedPort;

                            // A bit stubborn but there is no other way...
                            if(dstPort->getType() == WarehousePortType_t::E_PORT_RIGHT &&
                               srcPort->getType() == WarehousePortType_t::E_PORT_LEFT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO());
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_LEFT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_RIGHT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO());
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_LEFT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_TOP)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() + 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_TOP &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_LEFT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() - 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_RIGHT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_TOP)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() - 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_TOP &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_RIGHT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() + 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_LEFT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_BOTTOM)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() - 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_BOTTOM &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_LEFT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() + 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_RIGHT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_BOTTOM)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() + 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_BOTTOM &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_RIGHT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO() - 90);
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_LEFT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_MID)
                            {
                                srcItem->setOrientationAligned(dstItem->getO());
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_MID &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_LEFT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO());
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_RIGHT &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_MID)
                            {
                                srcItem->setOrientationAligned(dstItem->getO());
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }
                            else if(dstPort->getType() == WarehousePortType_t::E_PORT_MID &&
                                    srcPort->getType() == WarehousePortType_t::E_PORT_RIGHT)
                            {
                                srcItem->setOrientationAligned(dstItem->getO());
                                auto dstRect = dstItem->getRect();
                                auto srcRect = srcItem->getRect();

                                if(dstItem->getO() == -90 || dstItem->getO() == 270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() + dstRect.height());
                                }
                                else if(dstItem->getO() == 180 || dstItem->getO() == -180)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() + dstRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                                else if(dstItem->getO() == 90 || dstItem->getO() == -270)
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y() - srcRect.height());
                                }
                                else
                                {
                                    srcItem->moveBy(dstRect.topLeft().x() - srcRect.topLeft().x() - srcRect.width(),
                                                    dstRect.topLeft().y() - srcRect.topLeft().y());
                                }
                            }

                            /*auto newRect = srcItem->getRect();
                            auto xdiff = int(newRect.topLeft().x()) % 10;
                            auto ydiff = int(newRect.topLeft().y()) % 10;
                            auto xshift = xdiff > 5 ? 10 - xdiff : -xdiff;
                            auto yshift = ydiff > 5 ? 10 - ydiff : -ydiff;
                            srcItem->moveBy(xshift, yshift);*/

                            this->whConn = new UiWarehouseConnection_t(selectedPort, this);
                            selectedPort->setWhConn(this->whConn);
                            selectedPort->connect();
                            connect();
                            UiWarehouseLayout_t::getWhLayout().addWhConn(this->whConn);
                            //ui->reset();
                        }
                        else
                        {
                            QMessageBox::critical(nullptr, "Error", "Cannot connect incompatible types!");
                            Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Cannot connect incompatible types!");
                        }
                    }
                }
            }
        }

        void UiWarehousePort_t::updatePort(double x, double y, int32_t w, int32_t h)
        {
            this->mRect.setTopLeft(QPointF(x, y));
            this->mRect.setBottomRight(QPointF(x + w, y + h));

            this->mOrigin.setX(mRect.center().x());
            this->mOrigin.setY(mRect.center().y());
        }

        int32_t UiWarehousePort_t::getWhPortID() const
        {
            return this->whPortID;
        }

        UiWarehouseItem_t* UiWarehousePort_t::getWhItem() const
        {
            return this->whItem;
        }

        WarehousePortType_t UiWarehousePort_t::getType() const
        {
            return this->whPortType;
        }

        void UiWarehousePort_t::mark()
        {
            this->setRotatedPixmap(":/img/plug_sel.png");
        }

        void UiWarehousePort_t::unmark()
        {
            this->setRotatedPixmap(":/img/plug.png");
        }

        void UiWarehousePort_t::select()
        {
            selectedPort = this;
            mark();
        }

        void UiWarehousePort_t::unselect()
        {
            selectedPort = nullptr;
            unmark();
        }

        void UiWarehousePort_t::connect()
        {
            selectedPort = nullptr;
            mConnected = true;
            unmark();
        }

        bool UiWarehousePort_t::isConnected() const
        {
            return whConn;
        }

        UiWarehouseConnection_t* UiWarehousePort_t::getWhConn() const
        {
            return whConn;
        }

        void UiWarehousePort_t::setWhConn(UiWarehouseConnection_t* newConn)
        {
            if(this->whConn != nullptr)
            {
                QMessageBox::critical(nullptr, "Error", "Already connected!");
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "Already connected!");
            }
            else
            {
                mConnected = true;
                this->whConn = newConn;
            }
        }

        void UiWarehousePort_t::dump() const
        {
            std::cout << "    - [UI] Dump warehouse port ID <" << this->whPortID << "> on item ID <" << this->getWhItem()->getWhItemID() << ">" << std::endl;

            if(this->isConnected())
            {
                whConn->dump();
            }
        }

        void UiWarehousePort_t::disconnect()
        {
            if(this->isConnected())
            {
                mConnected = false;
                whConn = nullptr;
            }
        }

        void UiWarehousePort_t::setRotatedPixmap(const QString& p)
        {
            this->setPixmap(QPixmap(p));

            switch(whPortType)
            {
                case WarehousePortType_t::E_PORT_LEFT:
                    setPixmapRotation(270);
                    break;
                case WarehousePortType_t::E_PORT_RIGHT:
                    setPixmapRotation(90);
                    break;
                case WarehousePortType_t::E_PORT_BOTTOM:
                    setPixmapRotation(180);
                    break;
                case WarehousePortType_t::E_PORT_TOP:
                case WarehousePortType_t::E_PORT_MID:
                    setPixmapRotation(0);
                    break;
            }
        }

        void UiWarehousePort_t::expand(int32_t s)
        {
            mRect.setTopLeft(QPointF(mRect.topLeft().x() - s, mRect.topLeft().y() - s));
            mRect.setBottomRight(QPointF(mRect.bottomRight().x() + s, mRect.bottomRight().y() + s));
            this->setRect(mRect);
        }

        void UiWarehousePort_t::shrink(int32_t s)
        {
            mRect.setTopLeft(QPointF(mRect.topLeft().x() + s, mRect.topLeft().y() + s));
            mRect.setBottomRight(QPointF(mRect.bottomRight().x() - s, mRect.bottomRight().y() - s));
            this->setRect(mRect);
        }
    }
}