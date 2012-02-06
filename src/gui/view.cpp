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


#include "view.h"

View::View(QWidget *parent) :
    QGraphicsView(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setAlignment( Qt::AlignLeft | Qt::AlignTop );
}

void View::resizeEvent(QResizeEvent *event) {
    if (event) {
        QGraphicsView::resizeEvent(event);
    }
    m_scene->resize(event->size());
}

boost::shared_ptr<Scene> View::createScene(boost::shared_ptr<Picmi> game) {
    m_scene.reset(new Scene(game));
    setScene(m_scene.get());
    m_scene->resize(size());
    return m_scene;
}

void View::setPaused(bool paused) {
    setEnabled(!paused);
    m_scene->setPaused(paused);
}
