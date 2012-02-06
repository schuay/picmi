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


#ifndef DRAGMANAGER_H
#define DRAGMANAGER_H

#include <QPoint>

#include "scene.h"
#include "src/logic/picmi.h"

class Scene;

class DragManager
{
public:
    DragManager(boost::shared_ptr<Picmi> game, Scene *scene, QPoint start);

    void init(Board::State state);
    void move(int x, int y);

private:
    QPoint normCoordinates(int x, int y);

private:

    enum DragDirection {
        Horizontal,
        Vertical,
        Undefined
    };

    const boost::shared_ptr<Picmi> m_game;
    const QPoint m_start;
    Scene *m_scene;
    Board::State m_before, m_after, m_request;
    DragManager::DragDirection m_direction;
};

#endif // DRAGMANAGER_H
