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


#include "config.h"



#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>
#include <QCommandLineParser>

#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData about("picmi",
                      i18n("Picmi"),
                      QString::fromLatin1("%1.%2.%3").arg(VERSION_MAJOR)
                                         .arg(VERSION_MINOR)
                                         .arg(VERSION_PATCH)
                                         .toAscii(),
                      i18n("Picmi - a nonogram puzzle game"),
                      KAboutLicense::GPL_V2,
                      i18n("(c) 2012 - 2014 The Picmi Authors"),
                      "https://projects.kde.org/projects/kde/kdegames/picmi");
    about.addAuthor(i18n("Jakob Gruber"), i18n("Picmi Author"), "jakob.gruber@gmail.com");

    QApplication app(argc, argv);
    QCommandLineParser parser;
    KAboutData::setApplicationData(about);
    parser.addVersionOption();
    parser.addHelpOption();
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);
    KLocalizedString::setApplicationDomain("picmi");

    MainWindow *w = new MainWindow;
    w->show();
    
    return app.exec();
}
