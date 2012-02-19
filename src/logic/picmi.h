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


#ifndef PICMI_H
#define PICMI_H

#include "boardmap.h"
#include "boardstate.h"
#include "src/settings.h"
#include "elapsedtime.h"
#include "src/highscores.h"

struct StreakElement {
    int value;
    bool solved;
};

class IOHandler;

class Picmi
{
public:
    Picmi(std::shared_ptr<Settings> settings);

    int width() const;
    int height() const;
    int remainingBoxCount() const { return m_map->boxCount() - m_state->boxCount(); }
    bool outOfBounds(int x, int y) const;

    /* 0 <= x < width(); 0 <= y < height() */
    Board::State stateAt(int x, int y) const { return m_state->get(x, y); }
    void setState(int x, int y, Board::State state);

    void setPaused(bool paused);
    int elapsedSecs() const;

    bool won() const;
    std::shared_ptr<HighScore> endGame();

    void undo();

    std::vector<std::shared_ptr<struct StreakElement> > getRowStreak(int y) const;
    std::vector<std::shared_ptr<struct StreakElement> > getColStreak(int x) const;

private:
    std::vector<std::shared_ptr<struct StreakElement> > newStreak(const std::vector<int> &map) const;
    std::vector<std::shared_ptr<struct StreakElement> > processStreak(
            const std::vector<int> &map, std::shared_ptr<BoardState::LineInfo> state) const;

private:
    std::shared_ptr<BoardMap> m_map;
    std::shared_ptr<BoardState> m_state;
    std::shared_ptr<Settings> m_settings;
    std::shared_ptr<IOHandler> m_io_handler;

    ElapsedTime m_timer;
};

#endif // PICMI_H
