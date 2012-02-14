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


#ifndef ELAPSEDTIME_H
#define ELAPSEDTIME_H

#include <QTime>
#include <QDateTime>

class ElapsedTime
{
public:
    ElapsedTime();

    void start();
    void pause(bool paused);
    void stop();

    QTime elapsed() const;
    int elapsedMSecs() const;
    QDateTime starttime() const;

private:
    QTime m_timer;
    QDateTime m_start;
    int m_elapsed;
    bool m_paused, m_stopped;
};

#endif // ELAPSEDTIME_H
