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

#ifndef HIGHSCOREWINDOW_H
#define HIGHSCOREWINDOW_H

#include <QDialog>
#include <QAbstractTableModel>

#include "src/highscores.h"

namespace Ui {
class HighScoreWindow;
}

class ScoreTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /* scores: list of scores to display */
    ScoreTableModel(QList<std::shared_ptr<HighScore> > scores);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    enum ScoreColumn {
        Duration = 0,
        Date
    };

    QList<std::shared_ptr<HighScore> > m_scores;
};

class HighScoreWindow : public QDialog
{
    Q_OBJECT
public:
    /* high score windows display a list of scores in a particular game category.
      this can be either determined by passing a particular score current (additional infos will be displayed
      about current */
    explicit HighScoreWindow(std::shared_ptr<HighScores> scores, std::shared_ptr<HighScore> current, QWidget *parent = 0);

    /* or by settings, in which case no additional information is displayed */
    explicit HighScoreWindow(std::shared_ptr<HighScores> scores, const Settings &settings, QWidget *parent = 0);
    ~HighScoreWindow();
    
private:
    void prepareTable(QList<std::shared_ptr<HighScore> > &scores);

    Ui::HighScoreWindow *ui;

    std::shared_ptr<ScoreTableModel> m_model;
};

#endif // HIGHSCOREWINDOW_H
