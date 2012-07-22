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


#include "streakitem.h"

#include "src/constants.h"
#include "src/settings.h"

StreakItem::StreakItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent) :
    QGraphicsTextItem(parent), ReloadableItem(x, y), m_game(game)
{
    setEnabled(false);
    setZValue(ZVALUE_STREAKTEXT);
    setFont(Renderer::instance()->getFont(Renderer::Regular));

    connect(Settings::instance(), SIGNAL(settingChanged(Settings::SettingsType)),
            this, SLOT(settingChanged(Settings::SettingsType)));
}

void StreakItem::settingChanged(Settings::SettingsType type)
{
    switch (type) {
    case Settings::FontColorSolved:
    case Settings::FontColorUnsolved: setFontColors(); break;
    default: break;
    }
}

void StreakItem::setFontColors()
{
    m_color_solved = Settings::instance()->fontColorSolved();
    m_color_unsolved = Settings::instance()->fontColorUnsolved();
    refresh();
}

int StreakItem::padding(int tilesize) const {
    return tilesize / 5;
}

RowStreakItem::RowStreakItem(QSharedPointer<Picmi> game, int y, QGraphicsItem *parent) :
    StreakItem(0, y, game, parent)
{
    setFontColors();
}

void RowStreakItem::refresh() {
    QVector<QSharedPointer<Streaks::StreakElement> > streak = m_game->getRowStreak(m_y);
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

    setFont(Renderer::instance()->getFont(Renderer::Regular));

    QRectF rect = boundingRect();

    /* right align + a little padding */
    const int x = 0 - rect.width() - padding(tilesize);

    /* one per row, center text */
    const int y = tilesize * m_y + (tilesize - rect.height()) / 2;

    setPos(x, y);
}

ColStreakItem::ColStreakItem(QSharedPointer<Picmi> game, int x, QGraphicsItem *parent) :
    StreakItem(x, 0, game, parent)
{
    setFontColors();
}

void ColStreakItem::refresh() {
    QVector<QSharedPointer<Streaks::StreakElement> > streak = m_game->getColStreak(m_x);
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

    setFont(Renderer::instance()->getFont(Renderer::Regular));

    QRectF rect = boundingRect();

    const int x = tilesize * m_x + (tilesize - rect.width()) / 2;
    const int y = 0 - rect.height() - padding(tilesize);

    setPos(x, y);
}
