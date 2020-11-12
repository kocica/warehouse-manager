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
            : BaseShapeGraphicItem_t(x, y, w, h, BaseShapeGraphicItem_t::ITEM_RECTANGLE, s, parent)
            , ui(ui)
            , whItem(dynamic_cast<UiWarehouseItem_t*>(parent))
            , whPortID(id)
            , whPortType(t)
        {
            this->showHandles(false);

            // TODO: Show direction indicators
        }

        UiWarehousePort_t::~UiWarehousePort_t()
        {
            if (this->isConnected())
            {
                delete whConn;
            }
        }

        void UiWarehousePort_t::mousePressEvent(QGraphicsSceneMouseEvent* event)
        {
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
                                srcItem->setO(dstItem->getO());
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
                                srcItem->setO(dstItem->getO());
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
                                srcItem->setO(dstItem->getO() + 90);
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
                                srcItem->setO(dstItem->getO() - 90);
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
                                srcItem->setO(dstItem->getO() - 90);
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
                                srcItem->setO(dstItem->getO() + 90);
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
                                srcItem->setO(dstItem->getO() - 90);
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
                                srcItem->setO(dstItem->getO() + 90);
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
                                srcItem->setO(dstItem->getO() + 90);
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
                                srcItem->setO(dstItem->getO() - 90);
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
                                srcItem->setO(dstItem->getO());
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
                                srcItem->setO(dstItem->getO());
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
                                srcItem->setO(dstItem->getO());
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
                                srcItem->setO(dstItem->getO());
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

                            this->whConn = new UiWarehouseConnection_t(selectedPort, this);
                            selectedPort->setWhConn(this->whConn);
                            selectedPort->connect();
                            connect();
                            UiWarehouseLayout_t::getWhLayout().addWhConn(this->whConn);
                            ui->reset();
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
            this->mRect.setTopLeft(QPointF(this->mRect.topLeft().x() + x, this->mRect.topLeft().y() + y));
            this->mRect.setBottomRight(QPointF(this->mRect.bottomRight().x() + x, this->mRect.bottomRight().y() + y));

            (void)w;
            (void)h;
            //this->mRect.setWidth(w);
            //this->mRect.setHeight(h);

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

        void UiWarehousePort_t::select()
        {
            selectedPort = this;
            mSelected = true;
        }

        void UiWarehousePort_t::unselect()
        {
            selectedPort = nullptr;
            mSelected = false;
        }

        void UiWarehousePort_t::connect()
        {
            selectedPort = nullptr;
            mConnected = true;
        }

        bool UiWarehousePort_t::isConnected() const
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
    }
}
