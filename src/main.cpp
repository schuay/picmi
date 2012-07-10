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


#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>

#include "config.h"
#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData about ("picmi",
                      0,
                      ki18n("Picmi"),
                      QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH).toAscii(),
                      ki18n("Picmi - a nonogram puzzle game"),
                      KAboutData::License_GPL_V2,
                      ki18n("(c) 2012 The Picmi Authors"),
                      KLocalizedString(),
                      "http://www.github.com/schuay/picmi-rewrite");
    about.addAuthor(ki18n("Jakob Gruber"), ki18n("Picmi Author"), "jakob.gruber@gmail.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;
    KGlobal::locale()->insertCatalog( QLatin1String( "libkdegames" ));

    MainWindow *w = new MainWindow;
    w->show();
    
    return app.exec();
}
