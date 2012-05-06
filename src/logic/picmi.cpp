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


#include "picmi.h"

#include <assert.h>

#include "config.h"

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

void IOHandler::set(int x, int y, Board::State state) {
    switch (state) {
    case Board::Cross: setCross(x, y); break;
    case Board::Box: setBox(x, y); break;
    default: assert(0);
    }
}

void IOHandler::setCross(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: m_state->set(x, y, Board::Nothing); break;
    case Board::Box: break;
    case Board::Nothing: m_state->set(x, y, Board::Cross); break;
    default: assert(0);
    }
}

class IOHandlerNoHints : public IOHandler
{
public:
    IOHandlerNoHints(BoardMap *map, BoardState *state, ElapsedTime *timer) : IOHandler(map, state, timer) { }

protected:
    void setBox(int x, int y);
};


void IOHandlerNoHints::setBox(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: break;
    case Board::Box: m_state->set(x, y, Board::Nothing); break;
    case Board::Nothing: m_state->set(x, y, Board::Box); break;
    default: assert(0);
    }
}

class IOHandlerHints : public IOHandler
{
public:
    IOHandlerHints(BoardMap *map, BoardState *state, ElapsedTime *timer) : IOHandler(map, state, timer) { }

protected:
    void setBox(int x, int y);
};


void IOHandlerHints::setBox(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: break;
    case Board::Box: m_state->set(x, y, Board::Nothing); break;
    case Board::Nothing:
        if (m_map->get(x, y) == Board::Box) {
            m_state->set(x, y, Board::Box);
        } else {
            m_state->set(x, y, Board::Cross);
            m_timer->addPenaltyTime();
        }
        break;
    default: assert(0);
    }
}

Picmi::Picmi(std::shared_ptr<Settings> settings)
{
    int width, height;
    double density;
    bool prevent_mistakes;
    switch (settings->level()) {
#ifdef HAVE_KGDIFFICULTY
    case KgDifficultyLevel::Easy: width = height = 10; density = 0.55; prevent_mistakes = false; break;
    case KgDifficultyLevel::Medium: width = 15; height = 10; density = 0.55; prevent_mistakes = false; break;
    case KgDifficultyLevel::Hard: width = height = 15; density = 0.55; prevent_mistakes = false; break;
    case KgDifficultyLevel::Custom:
#else
    case KGameDifficulty::Easy: width = height = 10; density = 0.55; prevent_mistakes = false; break;
    case KGameDifficulty::Medium: width = 15; height = 10; density = 0.55; prevent_mistakes = false; break;
    case KGameDifficulty::Hard: width = height = 15; density = 0.55; prevent_mistakes = false; break;
    case KGameDifficulty::Configurable:
#endif
    default: width = settings->width(); height = settings->height(); density = settings->boxDensity();
        prevent_mistakes = settings->preventMistakes(); break;
    }

    m_map.reset(new BoardMap(width, height, density));
    m_state.reset(new BoardState(width, height));
    m_streaks.reset(new Streaks(m_map, m_state));

    if (prevent_mistakes) {
        m_io_handler.reset(new IOHandlerHints(m_map.get(), m_state.get(), &m_timer));
    } else {
        m_io_handler.reset(new IOHandlerNoHints(m_map.get(), m_state.get(), &m_timer));
    }

    m_timer.start();
}

Picmi::Picmi(std::shared_ptr<BoardMap> board) {
    m_map = board;
    m_state.reset(new BoardState(board->width(), board->height()));
    m_streaks.reset(new Streaks(m_map, m_state));
    m_io_handler.reset(new IOHandlerNoHints(m_map.get(), m_state.get(), &m_timer));
    m_timer.start();
}

bool Picmi::won() const {
    /* detect a win by comparing streaks.
       the reason we don't use the raw map and state is because we can't guarantee
       that our generated puzzles have exactly one solution, but we can work around
       this by ending the game once all streaks are marked solved. */

    for (int x = 0; x < width(); x++) {
        std::vector<std::shared_ptr<Streaks::StreakElement> > streak = getColStreak(x);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i]->solved) {
                return false;
            }
        }
    }

    for (int y = 0; y < height(); y++) {
        std::vector<std::shared_ptr<Streaks::StreakElement> > streak = getRowStreak(y);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i]->solved) {
                return false;
            }
        }
    }

    return true;
}

QPoint Picmi::undo() {
    QPoint coord = m_state->undo();
    m_streaks->update();
    emit stateChanged();
    return coord;
}

KScoreDialog::FieldInfo Picmi::endGame() {
    m_timer.stop();

    KScoreDialog::FieldInfo score;
    score[KScoreDialog::Score].setNum(m_timer.elapsedSecs());
    score[KScoreDialog::Time] = Time(m_timer.elapsedSecs()).toString();
    score[KScoreDialog::Date] = m_timer.startDate().toString("dd MMM yyyy hh:mm");

    return score;
}

int Picmi::height() const {
    return m_map->height();
}

int Picmi::width() const {
    return m_map->width();
}

bool Picmi::outOfBounds(int x, int y) const {
    return m_map->outOfBounds(x, y);
}

void Picmi::setPaused(bool paused) {
    m_timer.pause(paused);
}

int Picmi::elapsedSecs() const {
    return m_timer.elapsedSecs();
}

void Picmi::setState(int x, int y, Board::State state) {
    m_io_handler->set(x, y, state);
    m_streaks->update(x, y);
    emit stateChanged();
    if (m_state->boxCount() == m_map->boxCount() && won()) {
        expose();
        emit gameWon();
    }
}

void Picmi::expose() {
    for (int x = 0; x < width(); x++) {
        for (int y = 0; y < height(); y++) {
            if (m_state->get(x, y) == Board::Nothing) {
                m_state->set(x, y, Board::Cross);
            }
        }
    }
}

std::vector<std::shared_ptr<Streaks::StreakElement> > Picmi::getRowStreak(int y) const {
    return m_streaks->getRowStreak(y);
}
std::vector<std::shared_ptr<Streaks::StreakElement> > Picmi::getColStreak(int x) const {
    return m_streaks->getColStreak(x);
}
