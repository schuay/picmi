/**
 * selectboard.h
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef SELECTBOARD_H
#define SELECTBOARD_H

#include <kdialog.h>
#include <memory>

#include "ui_selectboardwindow.h"

class Level;
class LevelTableModel;

class SelectBoardWindow : public KDialog
{
    Q_OBJECT
public:
    SelectBoardWindow(QWidget *parent);

    virtual ~SelectBoardWindow();

    std::shared_ptr<Level> selectedBoard() const;

private slots:
    void selectedLevelChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void updateDetails(std::shared_ptr<Level> level);
    Ui::LevelSelectUi *ui;

    QList<std::shared_ptr<Level> > m_levels;
    std::shared_ptr<LevelTableModel> m_model;
};

#endif
