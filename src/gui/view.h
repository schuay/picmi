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


#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QResizeEvent>

#include "scene.h"
#include "src/logic/picmi.h"

class View : public QGraphicsView
{
public:
    explicit View(QWidget *parent = 0);

    /* sets up the scene and returns a pointer to it */
    std::shared_ptr<Scene> createScene(std::shared_ptr<Picmi> game);
    void setPaused(bool paused);

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    std::shared_ptr<Scene> m_scene;
};

#endif // VIEW_H
