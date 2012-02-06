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


#include "pausebanneritem.h"

#include "src/constants.h"

PauseBannerItem::PauseBannerItem(QGraphicsItem *parent) :
    QGraphicsSimpleTextItem(parent), ReloadableItem(0, 0)
{
    setText("PAUSED");
    m_font.reset(new QFont(FONT_NAME, 24));
    setFont(*m_font);
    setVisible(false);
    setZValue(ZVALUE_PAUSEBANNER);
}

void PauseBannerItem::reload(const QSize &size) {
    QRectF rect = boundingRect();
    setPos((size.width() - rect.width()) / 2,
           (size.height() - rect.height()) / 2);
}
