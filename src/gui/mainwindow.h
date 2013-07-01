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

#include "config.h"

#include <QTimer>
#include <highscore/kscoredialog.h>
#include <kaction.h>
#include <ktoggleaction.h>
#include <kxmlguiwindow.h>
#include <kgdifficulty.h>

#include "scene.h"
#include "src/logic/levelloader.h" /* TODO: Replace with Level forward declaration
                                    * with Qt 5 (see struct LineInfo) */
#include "src/logic/picmi.h"
#include "view.h"

class QLabel;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void startRandomGame();
    void restartGame();
    void togglePaused(bool paused);
    void settings();
    void gameWon();
    void undo();
    void hint();
    void solve();
    void saveState();
    void loadState();
    void highscores();
    void levelChanged(const KgDifficultyLevel* level);
    void updatePlayedTime();
    void updatePositions();
    void loadBoard();
    void toggleFullscreen(bool full_screen);

    /* Enable or disable undo/save state related actions. */
    void undoStackSizeChanged(int size);
    void saveStackSizeChanged(int size);

private:
    enum Mode {
        Random, /* board is randomly generated, highscores enabled */
        Preset  /* board is fixed, highscores disabled */
    };

    void startGame();
    void startPresetGame(QSharedPointer<Level> board);

    /* Once a game is finished, updates action states, stops running timers,
       and generally update the UI and state variables for a stopped game. */
    void finalizeGame();

    void restoreWindowState();
    void saveWindowState();
    void pauseGame();
    QSharedPointer<KScoreDialog> createScoreDialog();
    void setupActions();

    KAction *m_action_undo,
            *m_action_save_state,
            *m_action_load_state,
            *m_action_hint,
            *m_action_solve;
    KToggleAction *m_action_pause;
    QLabel *m_status_time, *m_status_position;
    View m_view;
    QSharedPointer<Picmi> m_game;
    QSharedPointer<Scene> m_scene;
    QTimer m_timer;

    const QString m_key_pos;

    bool m_in_progress;
    enum Mode m_mode;

    QSharedPointer<Level> m_current_level;
};

#endif // MAINWINDOW_H
