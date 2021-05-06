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
#include "UiWarehouseItemLocation.h"

namespace whm
{
    namespace gui
    {
        std::map<WarehouseItemType_t, std::vector<WarehouseItemType_t>> allowedCombinations =
        {
            { WarehouseItemType_t::E_CONVEYOR,           { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_LOCATION_SHELF, WarehouseItemType_t::E_CONVEYOR_HUB,
                                                           WarehouseItemType_t::E_WAREHOUSE_ENTRANCE, WarehouseItemType_t::E_WAREHOUSE_DISPATCH, WarehouseItemType_t::E_WAREHOUSE_BUFFER }},
            { WarehouseItemType_t::E_CONVEYOR_HUB,       { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_CONVEYOR_HUB, WarehouseItemType_t::E_LOCATION_SHELF }},
            { WarehouseItemType_t::E_LOCATION_SHELF,     { WarehouseItemType_t::E_CONVEYOR, WarehouseItemType_t::E_CONVEYOR_HUB }},
            { WarehouseItemType_t::E_WAREHOUSE_ENTRANCE, { WarehouseItemType_t::E_CONVEYOR }},
            { WarehouseItemType_t::E_WAREHOUSE_DISPATCH, { WarehouseItemType_t::E_CONVEYOR }},
            { WarehouseItemType_t::E_WAREHOUSE_BUFFER,   { WarehouseItemType_t::E_CONVEYOR }}
        };

        bool isWhItemCombinationAllowed(WarehouseItemType_t lhs, WarehouseItemType_t rhs)
        {
            std::vector<WarehouseItemType_t>& allowedTypes = allowedCombinations[lhs];

            return std::any_of(allowedTypes.begin(), allowedTypes.end(),
                               [&](WarehouseItemType_t& whItemType) -> bool
                               {
                                   return whItemType == rhs;
                               });
        }

        UiWarehouseItem_t::UiWarehouseItem_t(QGraphicsScene* s, MainWindow* ui, int32_t x, int32_t y, int32_t w, int32_t h, WarehouseItemType_t type)
            : BaseShapeGraphicItem_t(x, y, w, h, BaseShapeGraphicItem_t::ITEM_RECTANGLE, ui, s)
            , ui(ui)
            , scene(s)
            , heatRect(new QGraphicsRectItem())
            , itemInfo(new QGraphicsTextItem())
            , whItemID(UiWarehouseLayout_t::getWhLayout().getNextWhItemID())
            , whItemType(type)
        {
            scene->addItem(heatRect);
            scene->addItem(itemInfo);

            portSizeX = portSizeY = std::min(h, w) / 3;

            info = scene->addText(QString());

            setAcceptHoverEvents(true);

            infoTimeout.setSingleShot(true);

            QObject::connect(&infoTimeout, &QTimer::timeout, [&](){ info->hide(); });
        }

        void UiWarehouseItem_t::moveItem(qreal x, qreal y)
        {
            qreal w = this->mRect.width();
            qreal h = this->mRect.height();

            this->updateChildren(x, y);

            this->mRect.setTopLeft(QPoint(this->mRect.topLeft().x() + x, this->mRect.topLeft().y() + y));
            this->mRect.setBottomRight(QPoint(this->mRect.topLeft().x() + w, this->mRect.topLeft().y() + h));

            this->mOrigin.setX(this->mOrigin.x() + x);
            this->mOrigin.setY(this->mOrigin.y() + y);

            this->updateHandles();
        }

        bool UiWarehouseItem_t::isConnected() const
        {
            return std::any_of(whPorts.begin(), whPorts.end(), [](auto* p)
                                                               -> bool
                                                               {
                                                                   return p->isConnected();
                                                               });
        }

        UiWarehouseItem_t::~UiWarehouseItem_t()
        {
            for (UiWarehousePort_t* whPort : whPorts)
            {
                delete whPort;
            }

            whPorts.clear();

            delete heatRect;
            delete itemInfo;
            delete info;
        }

        WarehouseItemType_t UiWarehouseItem_t::getWhItemType() const
        {
            return whItemType;
        }

        UiWarehouseItem_t::UiWarehousePortContainer_t UiWarehouseItem_t::getWhPorts() const
        {
            return whPorts;
        }

        void UiWarehouseItem_t::eraseFromLayout()
        {
            UiWarehouseLayout_t::getWhLayout().eraseWhItem(this);
        }

        void UiWarehouseItem_t::removeWhItem()
        {
            eraseFromLayout();
            delete this;
        }

        void UiWarehouseItem_t::mousePressEvent(QGraphicsSceneMouseEvent *event)
        {
            info->hide();

            if (event->button() == Qt::LeftButton)
            {
                if(UiCursor_t::getCursor().getMode() == UiCursorMode_t::E_MODE_DELETE)
                {
                    removeWhItem();
                    return;
                }
                else if(UiCursor_t::getCursor().isItemSelected())
                {
                    ui->resetCursor();
                }
            }

            BaseGraphicItem_t::mousePressEvent(event);
        }

        void UiWarehouseItem_t::disconnect()
        {
            for (UiWarehousePort_t* whPort : whPorts)
            {
                if(whPort->isConnected())
                {
                    UiWarehouseConnection_t* whConn = whPort->getWhConn();

                    delete whConn;
                }
            }
        }

        void UiWarehouseItem_t::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
        {
            QString text;

            text += QString("<table style='background-color:#BD1E51;color:#F1B814'>");

            text += QString("<tr><td><b>Item ID: </b></td><td>") + QString::number(whItemID) + QString("</td></tr>");

            int32_t r = UiWarehouseLayout_t::getWhLayout().getRatio();

            switch(whItemType)
            {
                case WarehouseItemType_t::E_LOCATION_SHELF:
                {
                        text += QString("<tr><td><b>Item Type:</b></td><td> Location rack</td></tr>");
                        text += QString("<tr><td><b>Location slots:</b></td><td>(")
                             + QString::number(dynamic_cast<UiWarehouseItemLocation_t*>(this)->getSlotCountX()) + QString(",")
                             + QString::number(dynamic_cast<UiWarehouseItemLocation_t*>(this)->getSlotCountY()) + QString(")</td></tr>");

                        const auto& s = dynamic_cast<UiWarehouseItemLocation_t*>(this)->getSlots();
                        int32_t occupied{ 0 };
                        for(auto* slot : s)
                        {
                            if(!slot->getArticle().empty())
                            {
                                ++ occupied;
                            }
                        }
                        text += QString("<tr><td><b>Occupation level:</b></td><td>") + QString::number(occupied) + QString("/") + QString::number(s.size()) + QString("</td></tr>");
                        break;
                }
                case WarehouseItemType_t::E_CONVEYOR:
                        text += QString("<tr><td><b>Item Type:</b></td><td> Conveyor</td></tr>");
                        break;
                case WarehouseItemType_t::E_CONVEYOR_HUB:
                        text += QString("<tr><td><b>Item Type:</b></td><td> Conveyor hub</td></tr>");
                        break;
                case WarehouseItemType_t::E_WAREHOUSE_ENTRANCE:
                        text += QString("<tr><td><b>Item Type:</b></td><td> Entrance</td></tr>");
                        break;
                case WarehouseItemType_t::E_WAREHOUSE_DISPATCH:
                        text += QString("<tr><td><b>Item Type:</b></td><td> Dispatch</td></tr>");
                        break;
                case WarehouseItemType_t::E_WAREHOUSE_BUFFER:
                        text += QString("<tr><td><b>Item Type:</b></td><td> Buffer</td></tr>");
                        break;
            }

            text += QString("<tr><td><b>Coordinates: </b></td><td>(") + QString::number(getX()/r) + QString(",") + QString::number(getY()/r) + QString(")</td></tr>");
            text += QString("<tr><td><b>Dimensions: </b></td><td>") + QString::number(getW()/r) + QString("×") + QString::number(getH()/r) + QString("</td></tr>");
            text += QString("<tr><td><b>Orientation: </b></td><td>") + QString::number(getO()) + QString("°</td></tr>");
            text += QString("<tr><td><b>Workload: </b></td><td>") + QString::number(workload * 100) + QString("%</td></tr>");

            for(auto* whPort : whPorts)
            {
                if(whPort->isConnected())
                {
                    auto* whConn = whPort->getWhConn();
                    auto itemToID = whConn->getTo()->getWhItem()->getWhItemID();
                    auto itemFromID = whConn->getFrom()->getWhItem()->getWhItemID();

                    if(itemToID != whItemID)
                        text += QString("<tr><td><b>Connected to item: </b></td><td>") + QString::number(itemToID) + QString("</td></tr>");
                    if(itemFromID != whItemID)
                        text += QString("<tr><td><b>Connected to item: </b></td><td>") + QString::number(itemFromID) + QString("</td></tr>");
                }
            }

            text += QString("</table>");

            info->setPos(event->scenePos());
            info->setZValue(10);
            info->setFont(QFont("Helvetica", 20));
            info->setHtml(text);
            info->show();

            QGraphicsItem::hoverEnterEvent(event);
        }

        void UiWarehouseItem_t::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
        {
            info->hide(); //infoTimeout.start(500);

            QGraphicsItem::hoverLeaveEvent(event);
        }

        void UiWarehouseItem_t::removeItemHeat()
        {
            heatRect->setVisible(false);
        }

        /**
         * @author  John1024 (https://stackoverflow.com/users/3030305/john1024)
         * @date    Dec 26 '13 at 23:18
         * @link    Q https://stackoverflow.com/q/20792445/8254699
         * @link    A https://stackoverflow.com/a/20792531/8254699
         */
        void UiWarehouseItem_t::setItemHeat(double h)
        {
            double ratio = 2 * h;
            workload = h;

            static const auto& oorc = [](int32_t v){ return v < 0 || v > 255; };

            int32_t b = std::max(0.0, 255 * (1 - ratio));
            int32_t r = std::max(0.0, 255 * (ratio - 1));
            int32_t g = 255 - b - r;

            if(oorc(b) || oorc(r) || oorc(g))
            {
                Logger_t::getLogger().print(LOG_LOC, LogLevel_t::E_WARNING, "RGB value out of range!");
                return;
            }

            auto boundingRect = getRect();
            boundingRect.setTopLeft(QPointF(boundingRect.topLeft().x() - 10, boundingRect.topLeft().y() - 10));
            boundingRect.setBottomRight(QPointF(boundingRect.bottomRight().x() + 10, boundingRect.bottomRight().y() + 10));

            heatRect->setVisible(true);
            heatRect->setRect(boundingRect);
            heatRect->setBrush(QColor::fromRgb(r, g, b));
        }

        void UiWarehouseItem_t::removeItemInfo()
        {
            itemInfo->setVisible(false);
        }

        void UiWarehouseItem_t::setItemInfo(QString info)
        {
            QFont f;
            f.setBold(true);
            f.setPixelSize(60);

            QString htmlInfo;
            htmlInfo += "<p style='background-color:#BD1E51;color:#F1B814'>";
            htmlInfo += info;
            htmlInfo += "</p>";

            itemInfo->setFont(f);
            itemInfo->setZValue(1);
            itemInfo->setVisible(true);
            itemInfo->setHtml(htmlInfo);
            itemInfo->setDefaultTextColor(Qt::red);

            // Get rotation invariant position
            auto o = getO();
            setO(0);
            itemInfo->setPos(QPointF(getX(), getY()));
            setO(o);
        }

        void UiWarehouseItem_t::dump() const
        {
            std::cout << "==============================================" << std::endl;
            std::cout << "  [UI] Warehouse item ID <" << whItemID << "> Type <" << to_underlying(whItemType) << ">" << std::endl;
            std::cout << "==============================================" << std::endl;

            std::cout << "x: " << getX() << " y: " << getY() << " w: " << getW() << " h: " << getH() << " o: " << getO() << std::endl;

            std::for_each(whPorts.begin(), whPorts.end(),
                          [](UiWarehousePort_t* p)
                          {
                              p->dump();
                          });
        }
    }
}