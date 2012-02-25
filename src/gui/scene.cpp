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


#include "scene.h"

#include "src/constants.h"

Scene::Scene(std::shared_ptr<Picmi> game, QObject *parent) :
    QGraphicsScene(parent), m_game(game), m_position(0, 0)
{
    init();
}

void Scene::loadBanners() {
    m_pause_banner = new PauseBannerItem();
    m_items.push_back(m_pause_banner);
    addItem(m_pause_banner);

    m_time_banner = new TimeBannerItem(m_game->remainingBoxCount());
    m_group->addToGroup(m_time_banner);
    m_items.push_back(m_time_banner);
}

void Scene::loadBackground() {
    PixmapItem *p = new PixmapItem(Renderer::Background, 0, 0);
    m_items.push_back(p);
    addItem(p);
}

void Scene::updatePlayedTime() {
    m_time_banner->setTime(m_game->elapsedSecs());
}

void Scene::loadCells() {
    for (int y = 0; y < m_game->height(); y++) {
        for (int x = 0; x < m_game->width(); x++) {
            PixmapItem *p = new PixmapItem(Renderer::CellFrame, x, y);
            p->setZValue(ZVALUE_CELLFRAME);
            m_items.push_back(p);
            m_group->addToGroup(p);

            CellItem *q = new CellItem(x, y, m_game, this);
            m_items.push_back(q);
            m_cells.push_back(q);
            m_group->addToGroup(q);

            HighlightItem *item = new HighlightItem(Renderer::Highlight, x, y);
            m_items.push_back(item);
            m_highlights.push_back(item);
            m_group->addToGroup(item);
        }
    }
}

void Scene::loadStreaks() {
    for (int x = 0; x < m_game->width(); x++) {
        StreakVBackgroundItem *p = new StreakVBackgroundItem((x % 2) ? Renderer::Streak1 : Renderer::Streak2, x);
        m_items.push_back(p);
        m_group->addToGroup(p);

        ColStreakItem *q = new ColStreakItem(m_game, x);
        m_items.push_back(q);
        m_col_streaks.push_back(q);
        m_group->addToGroup(q);
    }

    for (int y = 0; y < m_game->height(); y++) {
        StreakHBackgroundItem *p = new StreakHBackgroundItem((y % 2) ? Renderer::Streak1 : Renderer::Streak2, y);
        m_items.push_back(p);
        m_group->addToGroup(p);

        RowStreakItem *q = new RowStreakItem(m_game, y);
        m_items.push_back(q);
        m_row_streaks.push_back(q);
        m_group->addToGroup(q);
    }
}

void Scene::loadDividers() {
    for (int x = SECTION_SIZE - 1; x < m_game->width() - 1; x += SECTION_SIZE) {
        for (int y = 0; y < m_game->height(); y++) {
            PixmapItem *item = new PixmapItem(Renderer::Divider, x, y);
            item->setZValue(ZVALUE_DIVIDER);
            m_items.push_back(item);
            m_group->addToGroup(item);
        }
    }

    for (int y = SECTION_SIZE - 1; y < m_game->height() - 1; y += SECTION_SIZE) {
        for (int x = 0; x < m_game->width(); x++) {
            PixmapItem *item = new PixmapItem(Renderer::Divider, x + 1, y);
            item->setRotation(90);
            item->setZValue(ZVALUE_DIVIDER);
            m_items.push_back(item);
            m_group->addToGroup(item);
        }
    }
}

void Scene::init() {
    Renderer::instance()->setTilesize(47);

    const int tilesize = Renderer::instance()->getTilesize();
    const int x_offset = Renderer::instance()->getXOffset();
    const int y_offset = Renderer::instance()->getYOffset();

    setSceneRect(QRectF(0, 0, m_game->width() * tilesize + x_offset,
                        m_game->height() * tilesize + y_offset));

    m_group = new QGraphicsItemGroup;

    loadBanners();
    loadBackground();
    loadCells();
    loadStreaks();
    loadDividers();

    m_group->setHandlesChildEvents(false);
    m_group->setPos(x_offset, y_offset);
    addItem(m_group);

    m_cells[0]->setFocus();
    updateHighlights();
}

void Scene::refresh() {
    for (int i = 0; i < (int)m_cells.size(); i++) {
        m_cells[i]->refresh();
    }
    for (int i = 0; i < (int)m_row_streaks.size(); i++) {
        m_row_streaks[i]->refresh();
    }
    for (int i = 0; i < (int)m_col_streaks.size(); i++) {
        m_col_streaks[i]->refresh();
    }
    m_time_banner->setRemainingBoxes(m_game->remainingBoxCount());
}

void Scene::setPaused(bool paused) {
    m_game->setPaused(paused);
    m_group->setVisible(!paused);
    m_time_banner->setVisible(!paused);
    m_pause_banner->setVisible(paused);
}

void Scene::press(int x, int y, Board::State state) {

    m_position.setX(x);
    m_position.setY(y);
    m_cells[xy_to_i(x, y)]->setFocus();
    updateHighlights();

    m_game->setState(x, y, state);

    m_cells[xy_to_i(x, y)]->refresh();
    m_row_streaks[y]->refresh();
    m_col_streaks[x]->refresh();

    m_time_banner->setRemainingBoxes(m_game->remainingBoxCount());

    if (m_game->won()) {
        updatePlayedTime(); /* make sure we display the same time as in high score */
        hideHighlights();

        emit gameWon();
    }
}

void Scene::hover(int x, int y) {
    m_position.setX(x);
    m_position.setY(y);

    m_cells[xy_to_i(x, y)]->setFocus();

    updateHighlights();
}

void Scene::updateHighlights() {
    for (int i = 0; i < (int)m_highlights.size(); i++) {
        m_highlights[i]->highlight(m_position);
    }
}

void Scene::hideHighlights() {
    for (int i = 0; i < (int)m_highlights.size(); i++) {
        m_highlights[i]->setVisible(false);
    }
}

void Scene::move(int dx, int dy) {
    int x = m_position.x() + dx;
    int y = m_position.y() + dy;
    while (x < 0) {
        x += m_game->width();
    }
    while (y < 0) {
        y += m_game->height();
    }
    hover(x % m_game->width(), y % m_game->height());
}

int Scene::xy_to_i(int x, int y) const {
    return y * m_game->width() + x;
}

void Scene::resize(const QSize &size) {
    const int grid = gridSize(size);

    Renderer::instance()->setTilesize(grid);
    setSceneRect(QRectF(0, 0, size.width(), size.height()));

    const int height = grid * m_game->height() + Renderer::instance()->getYOffset();
    const int width = grid * m_game->width() + Renderer::instance()->getXOffset();
    const int xoffset = (size.width() - width) / 2;
    const int yoffset = (size.height() - height) / 2;

    m_group->setPos(xoffset + Renderer::instance()->getXOffset(), yoffset + Renderer::instance()->getYOffset());

    for (int i = 0; i < (int)m_items.size(); i++) {
        m_items[i]->reload(size);
    }
}

int Scene::gridSize(const QSize &size) const {
    const int streak_grid_count = Renderer::instance()->getStreakGridCount();
    int grid = size.width() / (m_game->width() + streak_grid_count);

    if ((m_game->height() + streak_grid_count) * grid > size.height()) {
        grid = size.height() / (m_game->height() + streak_grid_count);
    }

    return grid;
}
