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


#ifndef HIGHLIGHTITEM_H
#define HIGHLIGHTITEM_H

#include "pixmapitem.h"

class HighlightItem : public PixmapItem
{
public:
    /* creates a highlight at field coordinates (x,y) */
    HighlightItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent = 0);

    /* switches current highlight on/off according to the currently
      focused point p */
    void highlight(const QPoint &p);
};

#endif // HIGHLIGHTITEM_H
