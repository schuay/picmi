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


#ifndef ELAPSEDTIME_H
#define ELAPSEDTIME_H

#include <QDateTime>

class Time
{
public:
    Time(int seconds) : m_seconds(seconds) { }
    QString toString(QString format = "%1:%2:%3") const;

private:
    const int m_seconds;

    static const int m_secs_per_minute = 60;
    static const int m_secs_per_hour = 60 * 60;
};

class ElapsedTime
{
public:
    ElapsedTime();

    /* start, stop, or (un)pause the timer */
    void start();
    void pause(bool paused);
    void stop();

    /* adds penalty time and increases the next penalty amount */
    void addPenaltyTime();

    /* return elapsed seconds the the datetime when start() was called */
    int elapsedSecs() const;
    QDateTime startDate() const;

private:
    int currentTimesliceSecs() const;

    QDateTime m_start_date;
    int m_elapsed;
    qint64 m_start;
    bool m_paused, m_stopped;

    int m_next_penalty;
    const int m_penalty_multiplier;
};

#endif // ELAPSEDTIME_H
