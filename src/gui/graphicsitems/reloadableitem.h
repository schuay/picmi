/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
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


#ifndef RELOADABLEITEM_H
#define RELOADABLEITEM_H

#include <QSize>

#include "src/gui/renderer.h"

class ReloadableItem
{
public:
    /* constructs item with field coordinates (x,y) */
    ReloadableItem(int x, int y);

    virtual ~ReloadableItem() { }

    /* Called with view dimensions when window has been resized and
       item needs to reload and reposition itself correctly. */
    virtual void reload(const QSize &size) = 0;
protected:
    const int m_x, m_y;
};

#endif // RELOADABLEITEM_H
