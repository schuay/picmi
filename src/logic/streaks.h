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

#ifndef STREAKS_H
#define STREAKS_H

#include <memory>
#include <vector>
#include <QVector> /* TODO: remove vector */

#include "boardmap.h"
#include "boardstate.h"

struct LineInfo;

class Streaks
{
public:
    struct StreakElement {
        int value;
        bool solved;
    };

    Streaks(std::shared_ptr<BoardMap> map, std::shared_ptr<BoardState> state);

    void update(int x, int y);
    void update();

    /* returns the request row/col streak. these contain the least information required by
       the frontend, which is (for each position within a streak): "which number is this",
       and "is this position solved" */
    std::vector<std::shared_ptr<Streaks::StreakElement> > getRowStreak(int y) const;
    std::vector<std::shared_ptr<Streaks::StreakElement> > getColStreak(int x) const;

private:

    void calcMapStreaks();


    std::shared_ptr<BoardMap> m_map;
    std::shared_ptr<BoardState> m_state;

    std::vector<std::vector<int> > m_map_row_streaks;
    std::vector<std::vector<int> > m_map_col_streaks;

    QVector<std::shared_ptr<LineInfo> > m_state_row_streaks;
    QVector<std::shared_ptr<LineInfo> > m_state_col_streaks;
};

#endif // STREAKS_H
