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


#ifndef PICMI_H
#define PICMI_H

#include <highscore/kscoredialog.h>

#include "boardmap.h"
#include "boardstate.h"
#include "streaks.h"
#include "src/settings.h"
#include "elapsedtime.h"

class IOHandler;

class Picmi : public QObject
{
    Q_OBJECT
public:

    Picmi(std::shared_ptr<Settings> settings);
    Picmi(std::shared_ptr<BoardMap> board);

    int width() const;
    int height() const;
    int remainingBoxCount() const { return m_map->boxCount() - m_state->boxCount(); }
    bool outOfBounds(int x, int y) const;

    /* 0 <= x < width(); 0 <= y < height() */
    Board::State stateAt(int x, int y) const { return m_state->get(x, y); }
    void setState(int x, int y, Board::State state);

    void setPaused(bool paused);
    int elapsedSecs() const;

    /* ends the current game and returns the current high score object */
    KScoreDialog::FieldInfo endGame();

    /* undo last action (if it exists) and return the changed coordinate. */
    QPoint undo();

    /* if a saved state exists, load it. otherwise, do nothing */
    void loadState() { m_state->loadState(); emit stateChanged(); }
    void saveState() { m_state->saveState(); }
    int currentStateAge() const { return m_state->currentStateAge(); }

    /* returns the request row/col streak. these contain the least information required by
      the frontend, which is (for each position within a streak): "which number is this",
      and "is this position solved" */
    QVector<std::shared_ptr<Streaks::StreakElement> > getRowStreak(int y) const;
    QVector<std::shared_ptr<Streaks::StreakElement> > getColStreak(int x) const;

signals:
    void gameWon();
    void stateChanged();

private:

    /* returns true if the game has been won */
    bool won() const;

private:
    std::shared_ptr<BoardMap> m_map;
    std::shared_ptr<BoardState> m_state;
    std::shared_ptr<IOHandler> m_io_handler;
    std::shared_ptr<Streaks> m_streaks;

    ElapsedTime m_timer;
};

#endif // PICMI_H
