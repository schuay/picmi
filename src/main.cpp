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
#include <k4aboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>

#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    K4AboutData about ("picmi",
                      0,
                      ki18n("Picmi"),
                      QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH).toAscii(),
                      ki18n("Picmi - a nonogram puzzle game"),
                      K4AboutData::License_GPL_V2,
                      ki18n("(c) 2012 - 2014 The Picmi Authors"),
                      KLocalizedString(),
                      "https://projects.kde.org/projects/kde/kdegames/picmi");
    about.addAuthor(ki18n("Jakob Gruber"), ki18n("Picmi Author"), "jakob.gruber@gmail.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;
    KLocalizedString::setApplicationDomain( "picmi" );

    MainWindow *w = new MainWindow;
    w->show();
    
    return app.exec();
}
