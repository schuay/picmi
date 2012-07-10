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


#ifndef RENDERER_H
#define RENDERER_H

#include <QFont>
#include <QPixmap>
#include <QString>
#include <QVector>
#include <QtSvg/QSvgRenderer>
#include <memory>

class Renderer
{
public:
    enum Resource {
        Transparent = 0,
        Background,
        CellFrame,
        Box,
        Cross,
        Highlight,
        Streak1,
        Streak2,
        Divider,
        OverviewBox,
        OverviewCross
    };

    enum FontSize {
        Tiny,
        Small,
        Regular,
        Large,
        Huge,
        FontSizeLength
    };

    static Renderer *instance();

    /* returns the pixmap representing the given resource at the
      current tilesize */
    QPixmap getPixmap(Renderer::Resource res) const;

    int getTilesize() const;
    int getOverviewTilesize() const;

    const QFont &getFont(enum FontSize size) const;

    /* returns the width of streak areas in pixels */
    int getYOffset() const;
    int getXOffset() const;

    /* returns the width of streak areas in tiles */
    int getStreakGridCount() const;

    /* 0 < board_width, board_height */
    void setSize(const QSize &size, int board_width, int board_height,
                 const QStringList &streaks);

private:
    Renderer();

    QPixmap getCachedPixmap(Renderer::Resource resource, int h, int w) const;
    void loadResources();

    /* calculates the largest tile size such that the entire game area fits
      into the provided window size */
    int gridSize(const QSize &size, int board_width, int board_height) const;

    /* Returns true if the given streaks fit into the current area specified
       by m_streak_count and m_tilesize. */
    bool streaksFit(const QStringList &streaks) const;

    void setFontSize();

private:

    int m_tilesize;
    int m_overview_tilesize;
    int m_streak_grid_count;

    QFont m_fonts[FontSizeLength];

    std::shared_ptr<QSvgRenderer> m_renderer;

    QVector<QString> m_names;
};

#endif // RENDERER_H
