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


#include "renderer.h"

#include <assert.h>
#include <iostream>
#include <QPixmapCache>
#include <QPainter>
#include <QFile>
#include <QDir>

#include "src/systemexception.h"
#include "src/outofboundsexception.h"
#include "config.h"

Renderer::Renderer() : m_streak_grid_count(6)
{
    loadResources();

    m_names << "transparent";
    m_names << "background";
    m_names << "cellframe";
    m_names << "box";
    m_names << "cross";
    m_names << "highlight";
    m_names << "streak1";
    m_names << "streak2";
    m_names << "divider";
}

void Renderer::loadResources() {
    QList<QString> paths;
    paths << QString("gfx/") << QString(FILEPATH);

    /* try loading first from working directory, then the system directory */
    for (int i = 0; i < paths.size(); i++) {
        const QString filenameSvg = QDir::toNativeSeparators(paths[i] + "picmi.svg");
        const QString filenameBg = QDir::toNativeSeparators(paths[i] + "background.jpg");

        if (!QFile::exists(filenameSvg) ||
                !QFile::exists(filenameBg)) {
            continue;
        }
        m_renderer.reset(new QSvgRenderer(filenameSvg));
        m_background = loadPixmap(filenameBg);
        break;
    }

    if (!m_background) {
        throw SystemException();
    }
}

boost::shared_ptr<QPixmap> Renderer::loadPixmap(QString path)
{
    boost::shared_ptr<QPixmap> pixmap(new QPixmap);
    if (!pixmap->load(path)) {
        throw SystemException();
    }
    return pixmap;
}

void Renderer::setTilesize(int tilesize) {
    if (tilesize < 0) {
        throw OutOfBoundsException();
    }
    m_tilesize = tilesize;
}

int Renderer::getFontSize(enum FontSize size) const {
    switch (size) {
    case Regular: return (m_tilesize - 10) * 0.5 + 5;
    case Large: return (m_tilesize - 10) * 0.75 + 7;
    default: assert(0); /* not used */
    }

    return 0;
}

Renderer *Renderer::instance() {
    static Renderer instance;
    return &instance;
}

QPixmap Renderer::getPixmap(Renderer::Resource resource) const {
    switch (resource) {
    case Background: return *m_background.get();
    case Streak1:
    case Streak2: return getCachedPixmap(resource, m_tilesize, m_tilesize * m_streak_grid_count);
    default: return getCachedPixmap(resource, m_tilesize, m_tilesize);
    }
}

QPixmap Renderer::getCachedPixmap(Renderer::Resource resource, int h, int w) const {
    QString key = QString("%1:%2x%3").arg(m_names[resource]).arg(w).arg(h);

    QPixmap pixmap;
    if (!QPixmapCache::find(key, pixmap)) {
        pixmap = QPixmap(w, h);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        m_renderer->render(&painter, m_names[resource], QRectF(0, 0, w, h));
        painter.end();
        QPixmapCache::insert(key, pixmap);
    }

    return pixmap;
}

int Renderer::getTilesize() const {
    return m_tilesize;
}

int Renderer::getYOffset() const {
    return m_streak_grid_count * m_tilesize;
}

int Renderer::getXOffset() const {
    return m_streak_grid_count * m_tilesize;
}

int Renderer::getStreakGridCount() const {
    return m_streak_grid_count;
}
