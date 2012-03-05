/**
 * selectboard.cpp
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

#include "selectboardwindow.h"

#include <klocale.h>
#include <kpushbutton.h>
#include <QAbstractTableModel>
#include <assert.h>

#include "src/logic/elapsedtime.h"
#include "src/logic/levelloader.h"

class LevelTableModel : public QAbstractTableModel
{
public:
    LevelTableModel(const QList<std::shared_ptr<Level> > &levels, QObject * parent = 0);

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    const QList<std::shared_ptr<Level> > &m_levels;

    enum Columns {
        Name,
        LevelSet,
        Difficulty,
        Size,
        Solved,
        ColumnCount /* not a real column */
    };
};

LevelTableModel::LevelTableModel(const QList<std::shared_ptr<Level> > &levels, QObject *parent) :
    QAbstractTableModel(parent), m_levels(levels)
{

}

int LevelTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_levels.size();
}

int LevelTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant LevelTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::TextAlignmentRole)) {
             return QVariant();
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    std::shared_ptr<Level> level= m_levels[index.row()];
    switch (index.column()) {
    case Name: return level->visibleName();
    case LevelSet: return level->levelset();
    case Difficulty: return level->difficulty();
    case Size: return QString("%1x%2").arg(level->width()).arg(level->height());
    case Solved: return "TODO";
    default: assert(0);
    }

    return QVariant();
}

QVariant LevelTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Name: return QVariant(ki18n("Level").toString());
        case LevelSet: return QVariant(ki18n("Level Set").toString());
        case Difficulty: return QVariant(ki18n("Difficulty").toString());
        case Size: return QVariant(ki18n("Size").toString());
        case Solved: return QVariant(ki18n("Solved").toString());
        default: assert(0);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

SelectBoardWindow::SelectBoardWindow(QWidget *parent)
    : KDialog(parent)
{
    setModal(true);
    setCaption("Level Selection");
    setMinimumSize(600, 350);

    ui = new Ui::LevelSelectUi;
    ui->setupUi(mainWidget());

    m_levels = LevelLoader::load();
    m_model.reset(new LevelTableModel(m_levels));
    ui->listView->setModel(m_model.get());

    if (m_levels.empty()) {
        button(KDialog::Ok)->setEnabled(false);
    } else {
        QModelIndex index = m_model->index(0, 0);
        ui->listView->selectionModel()->select(index, QItemSelectionModel::Select);
        connect(ui->listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedLevelChanged(QModelIndex,QModelIndex)));
        updateDetails(m_levels[0]);
    }
}

SelectBoardWindow::~SelectBoardWindow() {
    delete ui;
}

void SelectBoardWindow::selectedLevelChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);
    updateDetails(m_levels[current.row()]);
}

QString SelectBoardWindow::diffString(int difficulty) const {
    switch (difficulty) {
    case 0: return ki18n("RidiculouslyEasy").toString();
    case 1: return ki18n("VeryEasy").toString();
    case 2: return ki18n("Easy").toString();
    case 3: return ki18n("Medium").toString();
    case 4: return ki18n("Hard").toString();
    case 5: return ki18n("VeryHard").toString();
    case 6: return ki18n("ExtremelyHard").toString();
    case 7: return ki18n("Impossible").toString();
    default: throw OutOfBoundsException();
    }
}

void SelectBoardWindow::updateDetails(std::shared_ptr<Level> level) {
    ui->labelName->setText(QString("%1: %2").arg(ki18n("Name").toString(), level->visibleName()));
    ui->labelAuthor->setText(QString("%1: %2").arg(ki18n("Author").toString(), level->author()));
    ui->labelSize->setText(QString("%1: %2x%3").arg(ki18n("Size").toString()).arg(level->width()).arg(level->height()));
    ui->labelDifficulty->setText(QString("%1: %2").arg(ki18n("Difficulty").toString()).arg(diffString(level->difficulty())));
    if (level->solved()) {
        ui->labelSolved->setText(QString("%1: %2").arg(ki18n("Solved").toString()).arg(Time(level->solvedTime()).toString()));
    } else {
        ui->labelSolved->setText(QString("%1: %2").arg(ki18n("Solved").toString(), "-"));
    }
}

std::shared_ptr<Level> SelectBoardWindow::selectedBoard() const {
    int index = ui->listView->selectionModel()->selectedIndexes().at(0).row();
    return m_levels[index];
}
