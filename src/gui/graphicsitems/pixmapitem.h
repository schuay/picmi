/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
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


#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QGraphicsPixmapItem>

#include "reloadableitem.h"
#include "src/settings.h"

class PixmapItem : public QGraphicsPixmapItem, public ReloadableItem
{
public:
    PixmapItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent = 0);
    virtual void reload(const QSize &size);

protected:
    const Renderer::Resource m_resource;
};

class BackgroundItem : public QObject, public PixmapItem
{
    Q_OBJECT
public:
    BackgroundItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent = 0);

private slots:
    void settingChanged(Settings::SettingsType type);
};

class StreakHBackgroundItem : public PixmapItem
{
public:
    StreakHBackgroundItem(Renderer::Resource resource,int y, QGraphicsItem *parent = 0);
    virtual void reload(const QSize &size);
};

class StreakVBackgroundItem : public PixmapItem
{
public:
    StreakVBackgroundItem(Renderer::Resource resource,int x, QGraphicsItem *parent = 0);
    virtual void reload(const QSize &size);
};

#endif // PIXMAPITEM_H
