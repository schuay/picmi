/* *************************************************************************
 *  Copyright 2012 Jakob Gruber <jakob.gruber@gmail.com>                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#include "pixmapitem.h"

#include "src/constants.h"

PixmapItem::PixmapItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent), ReloadableItem(x, y), m_resource(resource)
{
    reload(QSize());
}

void PixmapItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    setPixmap(Renderer::instance()->getPixmap(m_resource));
    setPos(m_x * tilesize, m_y * tilesize);
}

StreakHBackgroundItem::StreakHBackgroundItem(Renderer::Resource resource, int y, QGraphicsItem *parent) :
    PixmapItem(resource, 0, y, parent)
{
    setZValue(ZVALUE_STREAKFRAME);
}

void StreakHBackgroundItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    const int offset = Renderer::instance()->getXOffset();

    setPixmap(Renderer::instance()->getPixmap(m_resource));
    setPos(0 - offset, m_y * tilesize);
}

StreakVBackgroundItem::StreakVBackgroundItem(Renderer::Resource resource, int x, QGraphicsItem *parent) :
    PixmapItem(resource, x, 0, parent)
{
    setZValue(ZVALUE_STREAKFRAME);
    rotate(90);
}

void StreakVBackgroundItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    const int offset = Renderer::instance()->getYOffset();

    setPixmap(Renderer::instance()->getPixmap(m_resource));
    setPos((m_x + 1) * tilesize, 0 - offset);
}
