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


#ifndef BOARDMAP_H
#define BOARDMAP_H

#include "board.h"

class BoardMap : public Board
{
public:
    /* 0 < width, height; 0.0 < box_ratio < 1.0 */
    BoardMap(int width, int height, double box_ratio);

    /* returns the actual streak for the given row/column;
      0 <= y < height, 0 <= x < width */
    std::vector<int> getRowStreak(int y) const;
    std::vector<int> getColStreak(int x) const;

    /* returns the total box count */
    int boxCount() const { return m_box_count; }

private:
    void genRandom();
    void calcStreaks();

    bool isStreakFiller(enum State state) const;

private:
    const int m_box_count;
    std::vector<std::vector<int> > m_row_streaks;
    std::vector<std::vector<int> > m_col_streaks;
};

#endif // BOARDMAP_H
