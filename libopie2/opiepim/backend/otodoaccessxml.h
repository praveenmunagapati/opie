/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_TODO_ACCESS_XML_H
#define OPIE_TODO_ACCESS_XML_H

#include <qasciidict.h>
#include <qmap.h>

#include <opie2/otodoaccessbackend.h>

namespace Opie {
    class XMLElement;

class OPimTodoAccessXML : public OPimTodoAccessBackend {
public:
    /**
     * fileName if Empty we will use the default path
     */
    OPimTodoAccessXML( const QString& appName,
                    const QString& fileName = QString::null );
    ~OPimTodoAccessXML();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    OPimTodo find( int uid )const;
    void clear();
    bool add( const OPimTodo& );
    bool remove( int uid );
    void removeAllCompleted();
    bool replace( const OPimTodo& );

    /* our functions */
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end,
                                bool includeNoDates )const;
    QArray<int> overDue()const;

//@{
    UIDArray sorted( const UIDArray&, bool, int, int, const QArray<int>& )const;
//@}
private:
    void todo( QAsciiDict<int>*, OPimTodo&,const QCString&,const QString& );
    QString toString( const OPimTodo& )const;
    QString toString( const QArray<int>& ints ) const;
    QMap<int, OPimTodo> m_events;
    QString m_file;
    QString m_app;
    bool m_opened : 1;
    bool m_changed : 1;
    class OPimTodoAccessXMLPrivate;
    OPimTodoAccessXMLPrivate* d;
    int m_year, m_month, m_day;
};

};

#endif
