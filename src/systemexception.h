/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
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


#ifndef SYSTEMEXCEPTION_H
#define SYSTEMEXCEPTION_H

#include <QString>
#include <exception>

class SystemException : public std::exception
{
public:
    SystemException() { m_msg = "system error"; }
    SystemException(const QString &msg) { m_msg = msg; }

    ~SystemException() throw() { }

    virtual const char *what() const throw() {
        return m_msg.toLatin1();
    }

private:
    QString m_msg;
};

#endif // SYSTEMEXCEPTION_H
