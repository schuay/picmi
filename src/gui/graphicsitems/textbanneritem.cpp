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


#include "textbanneritem.h"

#include <KLocalizedString>

#include "src/constants.h"

TextBannerItem::TextBannerItem(QGraphicsItem *parent) :
    QGraphicsTextItem(parent), ReloadableItem(0, 0)
{
    setEnabled(false);
    setFont(Renderer::instance()->getFont(Renderer::Large));
    setZValue(ZVALUE_BANNER);
}

void TextBannerItem::reload(const QSize &size) {
    Q_UNUSED(size);
    setFont(Renderer::instance()->getFont(Renderer::Large));
}

PauseBannerItem::PauseBannerItem(QGraphicsItem *parent) : TextBannerItem(parent)
{
    setPlainText(ki18n("PAUSED").toString());
    setVisible(false);
}

void PauseBannerItem::reload(const QSize &size) {
    TextBannerItem::reload(size);
    QRectF rect = boundingRect();
    setPos((size.width() - rect.width()) / 2,
           (size.height() - rect.height()) / 2);
}
