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


#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QVector>

#include "graphicsitems/cellitem.h"
#include "graphicsitems/highlightitem.h"
#include "graphicsitems/pixmapitem.h"
#include "graphicsitems/streakitem.h"
#include "graphicsitems/textbanneritem.h"
#include "renderer.h"
#include "src/logic/picmi.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(QSharedPointer<Picmi> game, QObject *parent = 0);

    /* 0 <= x < m_game.width(); 0 <= y < m_game.height()
      handles a request to (un)mark (x,y) as a box or cross */
    void press(int x, int y, Board::State state);
    /* sets focus and highlights to a specific cell,
      either specified using absolute coordinates (x,y)
      or relative coordinates (dx,dy) */
    void hover(int x, int y);
    void move(int dx, int dy);

    /* resize scene to view size */
    void resize(const QSize &size);

    /* refresh display of all dynamic graphics elements
       related to p. */
    void refresh(const QPoint &p);
    /* refresh display of all dynamic graphics elements */
    void refresh();

    void setPaused(bool paused);

    void forwardKeyPressEvent(QKeyEvent *event);

private slots:
    void onGameCompleted();

private:

    void init();
    void loadBackground();
    void loadStreaks();
    void loadCells();
    void loadDividers();
    void loadBanners();
    void loadOverView();
    void setGroupPos(const QSize &size);
    void setOverviewPos();

    void updateHighlights();
    void hideHighlights();
    int xy_to_i(int x, int y) const;

    QSharedPointer<Picmi> m_game;

    /* The scene automatically deletes registered items */
    PauseBannerItem *m_pause_banner;
    QVector<ReloadableItem*> m_items;
    QVector<GameCellItem*> m_cells;
    QVector<OverviewCellItem*> m_overview_cells;
    QVector<StreakItem*> m_row_streaks;
    QVector<StreakItem*> m_col_streaks;
    QVector<HighlightItem*> m_highlights;
    QGraphicsItemGroup *m_group;
    QGraphicsItemGroup *m_overview_group;

    /* All streaks as strings. Needed for calculation of streak
       area dimensions. */
    QStringList m_streak_strings;

    /* current position on board */
    QPoint m_position;
};

#endif // SCENE_H
