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


#include "textbanneritem.h"

#include "src/constants.h"

TextBannerItem::TextBannerItem(QGraphicsItem *parent) :
    QGraphicsTextItem(parent), ReloadableItem(0, 0)
{
    m_font.reset(new QFont(FONT_NAME, 24));
    setFont(*m_font);
    setZValue(ZVALUE_BANNER);
    m_size = Renderer::Large;
}

void TextBannerItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int fontsize = Renderer::instance()->getFontSize(m_size);
    m_font->setPointSize(fontsize);
    setFont(*m_font);
}

PauseBannerItem::PauseBannerItem(QGraphicsItem *parent) : TextBannerItem(parent)
{
    setPlainText("PAUSED");
    setVisible(false);
}

void PauseBannerItem::reload(const QSize &size) {
    TextBannerItem::reload(size);
    QRectF rect = boundingRect();
    setPos((size.width() - rect.width()) / 2,
           (size.height() - rect.height()) / 2);
}


TimeBannerItem::TimeBannerItem(int remaining_boxes, QGraphicsItem *parent) : TextBannerItem(parent)
{
    m_size = Renderer::Regular;
    setTime(QTime(0, 0, 0));
    setRemainingBoxes(remaining_boxes);
}

void TimeBannerItem::setRemainingBoxes(int remaining_boxes) {
    m_remaining_boxes = remaining_boxes;
    updateText();
}

void TimeBannerItem::setTime(const QTime &time) {
    m_time = time.toString("mm:ss");
    updateText();
}

void TimeBannerItem::updateText() {
    QString text("<p align=\"center\">Elapsed time<br>%1"
                 "<p align=\"center\">Boxes left<br>%2");
    setHtml(text.arg(m_time).arg(m_remaining_boxes));
}

void TimeBannerItem::reload(const QSize &size) {
    TextBannerItem::reload(size);
    QRectF rect = boundingRect();
    const int xoffset = Renderer::instance()->getXOffset();
    const int yoffset = Renderer::instance()->getYOffset();

    setPos(0 - xoffset + (xoffset - rect.width()) / 2,
           0 - yoffset + (yoffset - rect.height()) / 2);
}
