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

#include "highscorewindow.h"
#include "ui_highscorewindow.h"

#include "src/outofboundsexception.h"

ScoreTableModel::ScoreTableModel(QList<boost::shared_ptr<HighScore> > scores) {
    m_scores = scores;
}

int ScoreTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_scores.size();
}

int ScoreTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 2; /* datetime, played time */
}

QVariant ScoreTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Duration: return QVariant("Time played");
        case Date: return QVariant("Date");
        default: assert(0);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant ScoreTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
             return QVariant();
    }
    boost::shared_ptr<HighScore> score = m_scores[index.row()];
    switch (index.column()) {
    case Duration: return score->played().toString();
    case Date: return score->datetime();
    default: assert(0);
    }
}

static bool durationLessThan(const boost::shared_ptr<HighScore> &p1, const boost::shared_ptr<HighScore> &p2) {
    return (p1->playedSeconds() < p2->playedSeconds());
}

HighScoreWindow::HighScoreWindow(boost::shared_ptr<HighScores> scores, boost::shared_ptr<HighScore> current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HighScoreWindow)
{
    ui->setupUi(this);

    QList<boost::shared_ptr<HighScore> > category_scores = scores->scoresInCategory(current);
    qSort(category_scores.begin(), category_scores.end(), durationLessThan);

    m_model.reset(new ScoreTableModel(category_scores));

    ui->tableView->setModel(m_model.get());
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->show();
    ui->tableView->verticalHeader()->show();

    int current_index = -1, total_time = 0;
    for (int i = 0; i < category_scores.size(); i++) {
        total_time += category_scores[i]->playedSeconds();
        if (category_scores[i] == current) {
            current_index = i;
        }
    }
    if (current_index == -1) {
        throw OutOfBoundsException();
    }
    int average_time = total_time / category_scores.size();

    QString s("You placed #%1 out of %2\nTime used: %3\nAverage time used: %4");
    ui->label_2->setText(s.arg(current_index + 1)
                          .arg(category_scores.size())
                          .arg(current->played().toString())
                          .arg(QTime(0, 0, 0).addSecs(average_time).toString()));
}

HighScoreWindow::~HighScoreWindow()
{
    delete ui;
}
