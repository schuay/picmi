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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kxmlguiwindow.h>
#include <QTimer>
#include <highscore/kscoredialog.h>
#include <kaction.h>
#include <ktoggleaction.h>
#include <kgdifficulty.h>

#include "src/logic/picmi.h"
#include "scene.h"
#include "view.h"

class Level;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void startRandomGame();
    void togglePaused(bool paused);
    void settings();
    void gameWon();
    void undo();
    void highscores();
    void levelChanged(const KgDifficultyLevel* level);
    void updatePlayedTime();
    void loadBoard();
    void dumpBoard() const;

private:
    enum Mode {
        Random, /* board is randomly generated, highscores enabled */
        Preset  /* board is fixed, highscores disabled */
    };

    void startGame();
    void startPresetGame(std::shared_ptr<Level> board);

    void restoreWindowState();
    void saveWindowState();
    void pauseGame();
    std::shared_ptr<KScoreDialog> createScoreDialog();
    void setupActions();

    KAction *m_action_undo;
    KToggleAction *m_action_pause;
    View m_view;
    std::shared_ptr<Picmi> m_game;
    std::shared_ptr<Scene> m_scene;
    QTimer m_timer;

    const QString m_key_size, m_key_pos;
    const int m_statusbar_time_id;

    bool m_in_progress;
    enum Mode m_mode;

    std::shared_ptr<Level> m_current_level;
};

#endif // MAINWINDOW_H
