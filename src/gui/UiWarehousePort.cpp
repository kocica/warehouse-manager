/**
 * Warehouse manager
 *
 * @file    UiWarehousePort.cpp
 * @date    09/30/2020
 * @author  Filip Kocica
 * @brief   Graphical representation of warehouse port
 */

#include <iostream>

#include "UiCursor.h"
#include "UiWarehousePort.h"
#include "UiWarehouseItem.h"
#include "UiWarehouseLayout.h"
#include "UiWarehouseConnection.h"

namespace whm
{
    namespace gui
    {
        UiWarehousePort_t *UiWarehousePort_t::selectedPort = nullptr;

        UiWarehousePort_t::UiWarehousePort_t(QGraphicsScene *s, QGraphicsItem* parent, MainWindow *ui, int32_t id, int32_t x, int32_t y)
            : BaseShapeGraphicItem_t(x, y, 25, 25, BaseShapeGraphicItem_t::ITEM_RECTANGLE, s, parent)
            , ui(ui)
            , whItem(dynamic_cast<UiWarehouseItem_t*>(parent))
            , whPortID(id)
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
                        select();
                    }
                    else if (selectedPort->getWhItem() == this->getWhItem())
                    {
                        std::cout << "Same object!" << std::endl;
                    }
                    else
                    {
                        // TODO: Are object on the same level and/or connected?

                        if (isWhItemCombinationAllowed(this->getWhItem()->getWhItemType(),
                                               selectedPort->getWhItem()->getWhItemType()))
                        {
                            this->whConn = new UiWarehouseConnection_t(selectedPort, this);
                            selectedPort->setWhConn(this->whConn);
                            selectedPort->connect();
                            connect();
                            UiWarehouseLayout_t::getWhLayout().addWhConn(this->whConn);
                        }
                        else
                        {
                            std::cout << "Incompatible types!" << std::endl;
                        }
                    }
                }
            }
        }

        int32_t UiWarehousePort_t::getWhPortID() const
        {
            return whPortID;
        }

        UiWarehouseItem_t* UiWarehousePort_t::getWhItem() const
        {
            return this->whItem;
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
                std::cout << "Already connected!" << std::endl;
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
