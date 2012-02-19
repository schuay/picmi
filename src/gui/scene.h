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


#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <vector>

#include "src/logic/picmi.h"
#include "renderer.h"
#include "cellitem.h"
#include "streakitem.h"
#include "pixmapitem.h"
#include "highlightitem.h"
#include "textbanneritem.h"

class CellItem;

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(std::shared_ptr<Picmi> game, QObject *parent = 0);

    /* 0 <= x < m_game.width(); 0 <= y < m_game.height() */
    void press(int x, int y, Board::State state);
    void hover(int x, int y);
    void move(int dx, int dy);
    void resize(const QSize &size);

    void setPaused(bool paused);

    void refresh();
    
signals:
    void gameWon();
    
public slots:
    void updatePlayedTime();

private:

    void init();
    void loadBackground();
    void loadStreaks();
    void loadCells();
    void loadDividers();
    void loadBanners();

    void updateHighlights();
    void hideHighlights();
    int xy_to_i(int x, int y) const;
    int gridSize(const QSize &size) const;

    std::shared_ptr<Picmi> m_game;

    /* The scene automatically deletes registered items */
    TimeBannerItem *m_time_banner;
    PauseBannerItem *m_pause_banner;
    std::vector<ReloadableItem*> m_items;
    std::vector<CellItem*> m_cells;
    std::vector<StreakItem*> m_row_streaks;
    std::vector<StreakItem*> m_col_streaks;
    std::vector<HighlightItem*> m_highlights;
    QGraphicsItemGroup *m_group;

    /* current position on board */
    QPoint m_position;
};

#endif // SCENE_H
