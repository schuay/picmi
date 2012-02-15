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


#include "streakitem.h"

#include <QFont>

#include "src/constants.h"

StreakItem::StreakItem(int x, int y, boost::shared_ptr<Picmi> game, QGraphicsItem *parent) :
    QGraphicsTextItem(parent), ReloadableItem(x, y), m_game(game), m_padding(10), m_color_solved("#555555"), m_color_unsolved("#000000")
{
    setZValue(ZVALUE_STREAKTEXT);
    m_font.reset(new QFont(FONT_NAME, 24));
    setFont(*m_font);
}

int StreakItem::fontSize(int tilesize) const {
    if (tilesize < 24) {
        return 10;
    } else if (tilesize < 28) {
        return 12;
    } else if (tilesize < 32) {
        return 14;
    } else if (tilesize < 36) {
        return 16;
    } else if (tilesize < 40) {
        return 20;
    } else if (tilesize < 48) {
        return 24;
    } else {
        return 28;
    }
}

RowStreakItem::RowStreakItem(boost::shared_ptr<Picmi> game, int y, QGraphicsItem *parent) :
    StreakItem(0, y, game, parent)
{
    refresh();
}

void RowStreakItem::refresh() {
    std::vector<boost::shared_ptr<struct StreakElement> > streak = m_game->getRowStreak(m_y);
    QString text;

    for (int i = 0; i < (int)streak.size(); i++) {
        QString color = (streak[i]->solved ? m_color_solved : m_color_unsolved);
        text.append(QString("<font color=\"%1\">%2</font>").arg(color).arg(streak[i]->value));
        if (i != (int)streak.size() - 1) {
            text.append(" ");
        }
    }

    setHtml(text);
    reload(QSize());
}

void RowStreakItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();

    m_font->setPointSize(fontSize(tilesize));
    setFont(*m_font);

    QRectF rect = boundingRect();

    /* right align + a little padding */
    const int x = 0 - rect.width() - m_padding;

    /* one per row, center text */
    const int y = tilesize * m_y + (tilesize - rect.height()) / 2;

    setPos(x, y);
}

ColStreakItem::ColStreakItem(boost::shared_ptr<Picmi> game, int x, QGraphicsItem *parent) :
    StreakItem(x, 0, game, parent)
{
    refresh();
}

void ColStreakItem::refresh() {
    std::vector<boost::shared_ptr<struct StreakElement> > streak = m_game->getColStreak(m_x);
    QString text;

    for (int i = 0; i < (int)streak.size(); i++) {
        QString color = (streak[i]->solved ? m_color_solved : m_color_unsolved);
        text.append(QString("<font color=\"%1\">%2</font>").arg(color).arg(streak[i]->value));
        if (i != (int)streak.size() - 1) {
            text.append("<br/>");
        }
    }

    setHtml(text);
    reload(QSize());
}

void ColStreakItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    m_font->setPointSize(fontSize(tilesize));
    setFont(*m_font);

    QRectF rect = boundingRect();

    /* right align + a little padding */
    const int x = Renderer::instance()->getTilesize() * m_x + (tilesize - rect.width()) / 2;

    /* one per row, center text */
    const int y = 0 - boundingRect().height() - m_padding;

    setPos(x, y);
}
