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

#ifndef SELECTBOARD_H
#define SELECTBOARD_H

#include <QDialog>
#include <QSharedPointer>

#include "ui_selectboardwindow.h"

class Level;
class LevelTableModel;

class SelectBoardWindow : public QDialog
{
    Q_OBJECT
public:
    SelectBoardWindow(QWidget *parent);

    virtual ~SelectBoardWindow();

    QSharedPointer<Level> selectedBoard() const;

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void selectedLevelChanged(const QModelIndex &current, const QModelIndex &previous);
    void levelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    void updateDetails(QSharedPointer<Level> level);
    void resetSelection();

    Ui::LevelSelectUi *ui;

    QList<QSharedPointer<Level> > m_levels;
    QSharedPointer<LevelTableModel> m_model;
};

#endif
