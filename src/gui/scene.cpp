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

void Scene::forwardKeyPressEvent(QKeyEvent *event) {
    m_cells[xy_to_i(m_position.x(), m_position.y())]->keyPressEvent(event);
}

void Scene::loadBanners() {
    m_pause_banner = new PauseBannerItem();
    m_items.push_back(m_pause_banner);
    addItem(m_pause_banner);
}

void Scene::loadBackground() {
    PixmapItem *p = new PixmapItem(Renderer::Background, 0, 0);
    m_items.push_back(p);
    addItem(p);
}

void Scene::loadCells() {
    for (int y = 0; y < m_game->height(); y++) {
        for (int x = 0; x < m_game->width(); x++) {
            PixmapItem *p = new PixmapItem(Renderer::CellFrame, x, y);
            p->setZValue(ZVALUE_CELLFRAME);
            m_items.push_back(p);
            m_group->addToGroup(p);

            GameCellItem *q = new GameCellItem(x, y, m_game, this);
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

void Scene::loadOverView() {
    m_overview_group = new QGraphicsItemGroup;
    m_overview_group->setEnabled(false);
    for (int y = 0; y < m_game->height(); y++) {
        for (int x = 0; x < m_game->width(); x++) {
            OverviewCellItem *q = new OverviewCellItem(x, y, m_game);
            m_items.push_back(q);
            m_overview_cells.push_back(q);
            m_overview_group->addToGroup(q);
        }
    }
    m_group->addToGroup(m_overview_group);
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

        m_streak_strings.append(q->toPlainText());
    }

    for (int y = 0; y < m_game->height(); y++) {
        StreakHBackgroundItem *p = new StreakHBackgroundItem((y % 2) ? Renderer::Streak1 : Renderer::Streak2, y);
        m_items.push_back(p);
        m_group->addToGroup(p);

        RowStreakItem *q = new RowStreakItem(m_game, y);
        m_items.push_back(q);
        m_row_streaks.push_back(q);
        m_group->addToGroup(q);

        m_streak_strings.append(q->toPlainText());
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
    const int tilesize = Renderer::instance()->getTilesize();
    const int x_offset = Renderer::instance()->getXOffset();
    const int y_offset = Renderer::instance()->getYOffset();

    setSceneRect(QRectF(0, 0, m_game->width() * tilesize + x_offset,
                        m_game->height() * tilesize + y_offset));

    m_group = new QGraphicsItemGroup;

    loadBanners();
    loadBackground();
    loadCells();
    loadOverView();
    loadStreaks();
    loadDividers();

    m_group->setHandlesChildEvents(false);
    m_group->setPos(x_offset, y_offset);
    addItem(m_group);

    m_cells[0]->setFocus();
    updateHighlights();

    connect(m_game.get(), SIGNAL(gameWon()), this, SLOT(onGameWon()));
}

void Scene::refresh() {
    for (int i = 0; i < (int)m_cells.size(); i++) {
        m_cells[i]->refresh();
        m_overview_cells[i]->refresh();
    }
    for (int i = 0; i < (int)m_row_streaks.size(); i++) {
        m_row_streaks[i]->refresh();
    }
    for (int i = 0; i < (int)m_col_streaks.size(); i++) {
        m_col_streaks[i]->refresh();
    }
}

void Scene::refresh(const QPoint &p) {
    const int index = xy_to_i(p.x(), p.y());

    m_cells[index]->refresh();
    m_overview_cells[index]->refresh();
    m_row_streaks[p.y()]->refresh();
    m_col_streaks[p.x()]->refresh();
}

void Scene::setPaused(bool paused) {
    m_game->setPaused(paused);
    m_group->setVisible(!paused);
    m_pause_banner->setVisible(paused);
}

void Scene::press(int x, int y, Board::State state) {

    m_position.setX(x);
    m_position.setY(y);
    m_cells[xy_to_i(x, y)]->setFocus();
    updateHighlights();

    m_game->setState(x, y, state);

    refresh(QPoint(x, y));

    emit onAction();
}

void Scene::onGameWon() {
    hideHighlights();
    refresh();
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

void Scene::setOverviewPos() {
    int grid = Renderer::instance()->getOverviewTilesize();
    int streak_width = Renderer::instance()->getXOffset();
    int streak_height = Renderer::instance()->getYOffset();

    /* dimensions of overview */
    int height = grid * m_game->height();
    int width = grid * m_game->width();

    int xoffset = (streak_width - width) / 2;
    int yoffset = (streak_height - height) / 2;

    /*  */
    m_overview_group->setPos(xoffset - streak_width, yoffset - streak_height);

}

void Scene::setGroupPos(const QSize &size) {
    int grid = Renderer::instance()->getTilesize();
    int streak_width = Renderer::instance()->getXOffset();
    int streak_height = Renderer::instance()->getYOffset();

    /* dimensions of cell area + streaks */
    int height = grid * m_game->height() + streak_height;
    int width = grid * m_game->width() + streak_width;

    int xoffset = (size.width() - width) / 2;
    int yoffset = (size.height() - height) / 2;

    /* m_group contains all elements of the playing field. (0,0) is located
       at the left upper corner of the cells area, meaning streaks are located
       at negative coordinates.
       center the entire area within the available window size */
    m_group->setPos(xoffset + streak_width, yoffset + streak_height);

}

void Scene::resize(const QSize &size) {
    Renderer::instance()->setSize(size, m_game->width(), m_game->height(),
                                  m_streak_strings);
    setSceneRect(QRectF(0, 0, size.width(), size.height()));
    setGroupPos(size);
    setOverviewPos();

    for (int i = 0; i < (int)m_items.size(); i++) {
        m_items[i]->reload(size);
    }
}
