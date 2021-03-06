/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "statwindow.h"
#include <opie2/olistview.h>

using namespace Opie::Ui;
using namespace Opie::Ui;
using namespace Opie::Ui;
MStatWindow::MStatWindow( QWidget * parent, const char * name, WFlags f )
           :QVBox( parent, name, f )
{
    table = new OListView( this );
    table->addColumn( tr( "Protocol" ) );
    table->addColumn( tr( "Count" ) );
    table->setItemMargin( 2 );
};


void MStatWindow::updateCounter( const QString& protocol, int counter )
{
    QListViewItemIterator it( table );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->text( 0 ) == protocol )
        {
            it.current()->setText( 1, QString::number( counter ) );
            return;
        }
    }

    new OListViewItem( table, protocol, QString::number( counter ) );
}

