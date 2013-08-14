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
#include "elapsedtime.h"
#include "src/settings.h"
#include "streaks.h"

/* Moved from picmi.cpp to work around QSharedPointer issues with forward declarations.
 * TODO: move this back once when Qt 5 is used. */
class IOHandler
{
public:
    IOHandler(BoardMap *map, BoardState *state, ElapsedTime *timer) : m_map(map), m_state(state), m_timer(timer) { }
    virtual ~IOHandler() { }

    void set(int x, int y, Board::State state);

protected:
    virtual void setCross(int x, int y);
    virtual void setBox(int x, int y) = 0;

    BoardMap *m_map;
    BoardState *m_state;
    ElapsedTime *m_timer;
};

class Picmi : public QObject
{
    Q_OBJECT
public:

    Picmi();
    Picmi(QSharedPointer<BoardMap> board);

    int width() const;
    int height() const;
    int remainingBoxCount() const { return m_map->boxCount() - m_state->boxCount(); }
    QSharedPointer<BoardMap> getBoardMap() const { return m_map; }
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

    /* Uncovers a single, random, still uncovered cell. */
    QPoint hint();

    /* Solves the entire board, but does not emit the gameWon signal. */
    void solve();

    /* if a saved state exists, load it. otherwise, do nothing */
    void loadState() { m_state->loadState(); m_streaks->update(); emit stateChanged(); }
    void saveState() { m_state->saveState(); }
    int currentStateAge() const { return m_state->currentStateAge(); }

    /* returns the request row/col streak. these contain the least information required by
      the frontend, which is (for each position within a streak): "which number is this",
      and "is this position solved" */
    QVector<Streaks::Streak> getRowStreak(int y) const;
    QVector<Streaks::Streak> getColStreak(int x) const;

signals:
    /** Emitted when the game has been completed in any way. Also triggered if "Solve" was used. */
    void gameCompleted();

    /** Emitted when the game has actually been won (i.e. "Solve" wasn't used). */
    void gameWon();

    /**
     * Emitted whenever the board state has changed (i.e. by undoing an action,
     * setting a state explicitly, or loading a saved state).
     */
    void stateChanged();

    void undoStackSizeChanged(int size);
    void saveStackSizeChanged(int size);

private:

    /* returns true if the game has been won */
    bool won() const;

    void setupSlots();

private:
    QSharedPointer<BoardMap> m_map;
    QSharedPointer<BoardState> m_state;
    QSharedPointer<IOHandler> m_io_handler;
    QSharedPointer<Streaks> m_streaks;

    ElapsedTime m_timer;
};

#endif // PICMI_H
