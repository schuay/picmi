/* *************************************************************************
 *  Copyright 2012 Jakob Gruber                                            *
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


#ifndef PAUSEBANNERITEM_H
#define PAUSEBANNERITEM_H

#include <QGraphicsSimpleTextItem>
#include <QFont>

#include "reloadableitem.h"

class TextBannerItem : public QGraphicsTextItem, public ReloadableItem
{
public:
    TextBannerItem(QGraphicsItem *parent = 0);

    void reload(const QSize &size);

protected:
    std::shared_ptr<QFont> m_font;
    Renderer::FontSize m_size;
};

class TimeBannerItem : public TextBannerItem
{
public:
    TimeBannerItem(int remaining_boxes, QGraphicsItem *parent = 0);

    void setTime(int seconds);
    void setRemainingBoxes(int remaining_boxes);
    void reload(const QSize &size);

private:
    void updateText();

    QString m_time;
    int m_remaining_boxes;
};

class PauseBannerItem : public TextBannerItem
{
public:
    PauseBannerItem(QGraphicsItem *parent = 0);

    void reload(const QSize &size);
};

#endif // PAUSEBANNERITEM_H
