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

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <vector>

#include "src/logic/picmi.h"
#include "renderer.h"
#include "graphicsitems/cellitem.h"
#include "graphicsitems/streakitem.h"
#include "graphicsitems/pixmapitem.h"
#include "graphicsitems/highlightitem.h"
#include "graphicsitems/textbanneritem.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(std::shared_ptr<Picmi> game, QObject *parent = 0);

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

    void setPaused(bool paused);

    /* refresh display of all dynamic graphics elements */
    void refresh();

    void forwardKeyPressEvent(QKeyEvent *event);
    
signals:
    void gameWon();

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

    std::shared_ptr<Picmi> m_game;

    /* The scene automatically deletes registered items */
    PauseBannerItem *m_pause_banner;
    std::vector<ReloadableItem*> m_items;
    std::vector<GameCellItem*> m_cells;
    std::vector<OverviewCellItem*> m_overview_cells;
    std::vector<StreakItem*> m_row_streaks;
    std::vector<StreakItem*> m_col_streaks;
    std::vector<HighlightItem*> m_highlights;
    QGraphicsItemGroup *m_group;
    QGraphicsItemGroup *m_overview_group;

    /* current position on board */
    QPoint m_position;
};

#endif // SCENE_H
