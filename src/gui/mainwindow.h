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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <boost/shared_ptr.hpp>

#include "src/logic/picmi.h"
#include "scene.h"
#include "view.h"
#include "src/highscores.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void startGame();
    void togglePaused(bool paused);
    void help();
    void settings();
    void gameWon();
    void undo();
    void about();
    void highscores();

private:
    void restoreWindowState();
    void saveWindowState();
    void pauseGame();

    Ui::MainWindow *ui;

    boost::shared_ptr<Picmi> m_game;
    boost::shared_ptr<Scene> m_scene;
    boost::shared_ptr<HighScores> m_scores;
    QTimer m_timer;

    const QString m_key_size, m_key_pos;

    bool m_in_progress;
};

#endif // MAINWINDOW_H
