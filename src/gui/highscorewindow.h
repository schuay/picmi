#ifndef HIGHSCOREWINDOW_H
#define HIGHSCOREWINDOW_H

#include <QDialog>
#include <boost/shared_ptr.hpp>
#include <QAbstractTableModel>

#include "src/highscores.h"

namespace Ui {
class HighScoreWindow;
}

class ScoreTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ScoreTableModel(QList<boost::shared_ptr<HighScore> > scores);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    enum ScoreColumn {
        Duration = 0,
        Date
    };

    QList<boost::shared_ptr<HighScore> > m_scores;
};

class HighScoreWindow : public QDialog
{
    Q_OBJECT
public:
    explicit HighScoreWindow(boost::shared_ptr<HighScores> scores, boost::shared_ptr<HighScore> current, QWidget *parent = 0);
    ~HighScoreWindow();
    
private:
    Ui::HighScoreWindow *ui;

    boost::shared_ptr<ScoreTableModel> m_model;
};

#endif // HIGHSCOREWINDOW_H
