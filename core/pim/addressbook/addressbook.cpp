/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** OContact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QTOPIA_INTERNAL_FD

#include "contacteditor.h"
#include "ablabel.h"
#include "abview.h"
#include "abtable.h"
// #include "addresssettings.h"
#include "addressbook.h"


#include <opie/ofileselector.h>
#include <opie/ofiledialog.h>
#include <opie/ocontact.h>

#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/ir.h>
#include <qpe/qpemessagebox.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qaction.h>
#include <qdialog.h>
#include <qdir.h>
#include <qfile.h>
#include <qimage.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <qdatetime.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


#include "picker.h"
#include "configdlg.h"

static QString addressbookPersonalVCardName()
{
	QString filename = Global::applicationFileName("addressbook",
						       "businesscard.vcf");
	return filename;
}


AddressbookWindow::AddressbookWindow( QWidget *parent, const char *name,
				      WFlags f )
	: QMainWindow( parent, name, f ),
	  catMenu (0l),
	  abEditor(0l),
	  syncing(FALSE)
{
	isLoading = true;

	m_config.load();

	initFields();
	
	setCaption( tr("Contacts") );
	setIcon( Resource::loadPixmap( "AddressBook" ) );
	
	setToolBarsMovable( FALSE );
	
	// Create Toolbars
	
	QPEToolBar *bar = new QPEToolBar( this );
	bar->setHorizontalStretchable( TRUE );
	
	QPEMenuBar *mbList = new QPEMenuBar( bar );
	mbList->setMargin( 0 );
	
	QPopupMenu *edit = new QPopupMenu( this );
	mbList->insertItem( tr( "Contact" ), edit );
	
	listTools = new QPEToolBar( this, "list operations" );
	
	
	QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null,
				  0, this, 0 );
	actionNew = a;
	connect( a, SIGNAL( activated() ), this, SLOT( slotListNew() ) );
	a->addTo( edit );
	a->addTo( listTools );
	
	a = new QAction( tr( "Edit" ), Resource::loadPixmap( "edit" ), QString::null,
			 0, this, 0 );
	actionEdit = a;
	connect( a, SIGNAL( activated() ), this, SLOT( slotViewEdit() ) );
	a->addTo( edit );
	a->addTo( listTools );
	
	a = new QAction( tr( "Delete" ), Resource::loadPixmap( "trash" ), QString::null,
			 0, this, 0 );
	actionTrash = a;
	connect( a, SIGNAL( activated() ), this, SLOT( slotListDelete() ) );
	a->addTo( edit );
	a->addTo( listTools );
	
	
	// make it possible to go directly to businesscard via qcop call
#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
	QCopChannel *addressChannel = new QCopChannel("QPE/Addressbook" , this );
	connect (addressChannel, SIGNAL( received(const QCString &, const QByteArray &)),
		 this, SLOT ( appMessage(const QCString &, const QByteArray &) ) );
#endif
#endif
	a = new QAction( tr( "Find" ), Resource::loadPixmap( "mag" ),
			 QString::null, 0, this, 0 );
	actionFind = a;
	connect( a, SIGNAL(activated()), this, SLOT( slotFindOpen()) );
	a->addTo( edit );
	a->addTo( listTools );

	// Much better search widget, taken from QTReader.. (se)
	searchBar = new OFloatBar( "Search", this, QMainWindow::Top, TRUE );
	searchBar->setHorizontalStretchable( TRUE );
	searchBar->hide();
	searchEdit = new QLineEdit( searchBar, "searchEdit" );

// 	QFont f("unifont", 16 /*, QFont::Bold*/);
// 	searchEdit->setFont( f );

	searchBar->setStretchableWidget( searchEdit );
	connect( searchEdit, SIGNAL( returnPressed( ) ),
		 this, SLOT( slotFind( ) ) );

	a = new QAction( tr( "Start Search" ), Resource::loadPixmap( "enter" ), QString::null, 0, this, 0 );
	connect( a, SIGNAL( activated() ), this, SLOT( slotFind() ) );
	a->addTo( searchBar );

	a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
	connect( a, SIGNAL( activated() ), this, SLOT( slotFindClose() ) );
	a->addTo( searchBar );

	a = new QAction( tr( "Write Mail To" ), Resource::loadPixmap( "mail/sendmail" ),
			 QString::null, 0, this, 0 );
	//a->setEnabled( FALSE ); we got support for it now :) zecke
	actionMail = a;
	connect( a, SIGNAL( activated() ), this, SLOT( writeMail() ) );
	a->addTo( edit );
	a->addTo( listTools );
	
	if ( Ir::supported() ) {
		a = new QAction( tr ("Beam Entry" ), Resource::loadPixmap( "beam" ), QString::null,
				 0, this, 0 );
		actionBeam = a;
		connect( a, SIGNAL( activated() ), this, SLOT( slotBeam() ) );
		a->addTo( edit );
		a->addTo( listTools );
	}
	
	edit->insertSeparator();
	
	a = new QAction( tr("Import vCard"), QString::null, 0, 0, 0, TRUE );
	actionPersonal = a;
	connect( a, SIGNAL( activated() ), this, SLOT( importvCard() ) );
	a->addTo( edit );
	
	edit->insertSeparator();
	
	a = new QAction( tr("My Personal Details"), QString::null, 0, 0, 0, TRUE );
	actionPersonal = a;
	connect( a, SIGNAL( activated() ), this, SLOT( slotPersonalView() ) );
	a->addTo( edit );
	

#ifdef __DEBUG_RELEASE
	// Remove this function for public Release ! This is only
	// for debug purposes ..
	a = new QAction( tr( "Save all Data"), QString::null, 0, 0 );
	connect( a, SIGNAL( activated() ), this , SLOT( slotSave() ) );
	a->addTo( edit );
#endif
	a = new QAction( tr( "Config" ), Resource::loadPixmap( "today/config" ), QString::null,
			 0, this, 0 );
	connect( a, SIGNAL( activated() ), this, SLOT( slotConfig() ) );
	a->addTo( edit );

	// Create Views
	listContainer = new QWidget( this );
	QVBoxLayout *vb = new QVBoxLayout( listContainer );
	
 	m_abView = new AbView( listContainer, m_config.orderList() );
 	vb->addWidget( m_abView );
	// abList->setHScrollBarMode( QScrollView::AlwaysOff );
// :SX
// 	connect( abList, SIGNAL( empty( bool ) ), this, SLOT( listIsEmpty( bool ) ) );
// 	connect( abList, SIGNAL( details() ), this, SLOT( slotListView() ) );
// 	connect( abList, SIGNAL( currentChanged(int,int) ), this, SLOT( slotUpdateToolbar() ) );

	connect( m_abView, SIGNAL( signalViewSwitched ( int ) ), 
		 this, SLOT( slotViewSwitched( int ) ) );

	
	QObject::connect( m_abView, SIGNAL(signalNotFound()), this, SLOT(slotNotFound()) );
	
	m_abView->load();
	
	// Letter Picker
	pLabel = new LetterPicker( listContainer );
	connect(pLabel, SIGNAL(letterClicked(char)), this, SLOT(slotSetLetter(char)));
	vb->addWidget( pLabel );

	// Category Menu
	catMenu = new QPopupMenu( this );
	catMenu->setCheckable( TRUE );
	connect( catMenu, SIGNAL(activated(int)), this, SLOT(slotSetCategory(int)) );
	populateCategories();
	
	mbList->insertItem( tr("View"), catMenu );
	
 	defaultFont = new QFont( m_abView->font() );
 	slotSetFont(m_config.fontSize());
	m_curFontSize = m_config.fontSize();
	
	setCentralWidget(listContainer);
	
	//    qDebug("adressbook contrsuction: t=%d", t.elapsed() );

	
	isLoading = false;
}


void AddressbookWindow::slotConfig()
{
	ConfigDlg* dlg = new ConfigDlg( this, "Config" );
	dlg -> setConfig( m_config );
	dlg -> showMaximized();
	if ( dlg -> exec() ) {
		qWarning ("Config Dialog accepted !");
		m_config = dlg -> getConfig();
		if ( m_curFontSize != m_config.fontSize() ){
			qWarning("Font was changed!");
			m_curFontSize = m_config.fontSize();
			emit slotSetFont( m_curFontSize );
		}
		m_abView -> setListOrder( m_config.orderList() );
	}

	delete dlg;
}


void AddressbookWindow::slotSetFont( int size ) 
{
	qWarning("void AddressbookWindow::slotSetFont( %d )", size);
	
	if (size > 2 || size < 0)
		size = 1;
	
	m_config.setFontSize( size );
	
	QFont *currentFont;
	
	switch (size) {
	case 0:
		m_abView->setFont( QFont( defaultFont->family(), defaultFont->pointSize() - 2 ) );
		currentFont = new QFont (m_abView->font());
		// abList->resizeRows(currentFont->pixelSize() + 7); :SX
		// abList->resizeRows();
		break;
	case 1:
 		m_abView->setFont( *defaultFont );
 		currentFont = new QFont (m_abView->font());
// 		// abList->resizeRows(currentFont->pixelSize() + 7);
// 		abList->resizeRows();
		break;
	case 2:
 		m_abView->setFont( QFont( defaultFont->family(), defaultFont->pointSize() + 2 ) );
 		currentFont = new QFont (m_abView->font());
// 		//abList->resizeRows(currentFont->pixelSize() + 7);
// 		abList->resizeRows();
		break;
	}
}



void AddressbookWindow::importvCard() {
        QString str = OFileDialog::getOpenFileName( 1,"/");//,"", "*", this );
        if(!str.isEmpty() )
		setDocument((const QString&) str );
	
}

void AddressbookWindow::setDocument( const QString &filename )
{
	if ( filename.find(".vcf") != int(filename.length()) - 4 ) 
		return;
	
	QValueList<OContact> cl = OContact::readVCard( filename );
	for( QValueList<OContact>::Iterator it = cl.begin(); it != cl.end(); ++it ) {
		m_abView->addEntry( *it );
	}
	
}

void AddressbookWindow::resizeEvent( QResizeEvent *e )
{
	QMainWindow::resizeEvent( e );
	

}

AddressbookWindow::~AddressbookWindow()
{
	m_config.save();
}

void AddressbookWindow::slotUpdateToolbar()
{
 	OContact ce = m_abView->currentEntry();
	actionMail->setEnabled( !ce.defaultEmail().isEmpty() );
}

void AddressbookWindow::slotListNew()
{
	OContact cnt;
	if( !syncing ) {
		editEntry( NewEntry );
	} else {
		QMessageBox::warning(this, tr("OContacts"),
				     tr("Can not edit data, currently syncing"));
	}
}

// void AddressbookWindow::slotListView()
// {
// 	m_abView -> init( abList->currentEntry() );
// 	// :SX mView->sync();
// 	//:SX	showView();
// }

void AddressbookWindow::slotListDelete()
{
	if(!syncing) {
		OContact tmpEntry = m_abView ->currentEntry();
		
		// get a name, do the best we can...
		QString strName = tmpEntry.fullName();
		if ( strName.isEmpty() ) {
			strName = tmpEntry.company();
			if ( strName.isEmpty() )
				strName = "No Name";
		}
		
		
		if ( QPEMessageBox::confirmDelete( this, tr( "Contacts" ),
						   strName ) ) {
			m_abView->removeEntry( tmpEntry.uid() );
		}
	} else {
		QMessageBox::warning( this, tr("Contacts"),
				      tr("Can not edit data, currently syncing") );
	}
}

void AddressbookWindow::slotFindOpen()
{
	searchBar->show();
	m_abView -> inSearch();
	searchEdit->setFocus();
}
void AddressbookWindow::slotFindClose()
{
	searchBar->hide();
	m_abView -> offSearch();
	// m_abView->setFocus();
}


void AddressbookWindow::slotFind()
{
	m_abView->slotDoFind( searchEdit->text(), m_config.beCaseSensitive(), m_config.useRegExp(), false);
	
	searchEdit->clearFocus();
	// m_abView->setFocus();
	
}

void AddressbookWindow::slotViewBack()
{
	// :SX showList();
}

void AddressbookWindow::slotViewEdit()
{
	if(!syncing) {
		if (actionPersonal->isOn()) {
			editPersonal();
		} else {
			editEntry( EditEntry );
		}
	} else {
		QMessageBox::warning( this, tr("Contacts"),
				      tr("Can not edit data, currently syncing") );
	}
}



void AddressbookWindow::writeMail()
{
	OContact c = m_abView -> currentEntry();
	QString name = c.fileAs();
	QString email = c.defaultEmail();

	// I prefer the OPIE-Environment variable before the 
	// QPE-one..
	QString basepath = QString::fromLatin1( getenv("OPIEDIR") );
	if ( basepath.isEmpty() )
		basepath = QString::fromLatin1( getenv("QPEDIR") );

	// Try to access the preferred. If not possible, try to 
	// switch to the other one.. 
	if ( m_config.useQtMail() ){
		qWarning ("Accessing: %s", (basepath + "/bin/qtmail").latin1());
	        if ( QFile::exists( basepath + "/bin/qtmail" ) ){
			qWarning ("QCop");
			QCopEnvelope e("QPE/Application/qtmail", "writeMail(QString,QString)");
			e << name << email;
			return;
		} else
			m_config.setUseOpieMail( true );
	}
	if ( m_config.useOpieMail() ){
		qWarning ("Accessing: %s", (basepath + "/bin/mail").latin1());
		if ( QFile::exists( basepath + "/bin/mail" ) ){
			qWarning ("QCop");
			QCopEnvelope e("QPE/Application/mail", "writeMail(QString,QString)");
			e << name << email;
			return;
		} else
			m_config.setUseQtMail( true );
	}

}

static const char * beamfile = "/tmp/obex/contact.vcf";

void AddressbookWindow::slotBeam()
{
	QString filename;
	OContact c;
	if ( actionPersonal->isOn() ) {
		filename = addressbookPersonalVCardName();
		if (!QFile::exists(filename))
			return; // can't beam a non-existent file
		c = OContact::readVCard( filename )[0];
	} else {
		unlink( beamfile ); // delete if exists
		c = m_abView -> currentEntry();
		mkdir("/tmp/obex/", 0755);
		OContact::writeVCard( beamfile, c );
		filename = beamfile;
	}
	Ir *ir = new Ir( this );
	connect( ir, SIGNAL( done( Ir * ) ), this, SLOT( beamDone( Ir * ) ) );
	QString description = c.fullName();
	ir->send( filename, description, "text/x-vCard" );
}

void AddressbookWindow::beamDone( Ir *ir )
{
	delete ir;
	unlink( beamfile );
}


static void parseName( const QString& name, QString *first, QString *middle,
		       QString * last )
{
	
	int comma = name.find ( "," );
	QString rest;
	if ( comma > 0 ) {
		*last = name.left( comma );
		comma++;
		while ( comma < int(name.length()) && name[comma] == ' ' )
			comma++;
		rest = name.mid( comma );
	} else {
		int space = name.findRev( ' ' );
		*last = name.mid( space+1 );
		rest = name.left( space );
	}
	int space = rest.find( ' ' );
	if ( space <= 0 ) {
		*first = rest;
	} else {
		*first = rest.left( space );
		*middle = rest.mid( space+1 );
	}
	
}


void AddressbookWindow::appMessage(const QCString &msg, const QByteArray &data)
{
	if (msg == "editPersonal()") {
		editPersonal();
	} else if (msg == "editPersonalAndClose()") {
		editPersonal();
		close();
	} else if ( msg == "addContact(QString,QString)" ) {
		QDataStream stream(data,IO_ReadOnly);
		QString name, email;
		stream >> name >> email;
		
		OContact cnt;
		QString fn, mn, ln;
		parseName( name, &fn, &mn, &ln );
		//  qDebug( " %s - %s - %s", fn.latin1(), mn.latin1(), ln.latin1() );
		cnt.setFirstName( fn );
		cnt.setMiddleName( mn );
		cnt.setLastName( ln );
		cnt.insertEmails( email );
		cnt.setDefaultEmail( email );
		cnt.setFileAs();
		
		m_abView -> addEntry( cnt );
		
		// :SXm_abView()->init( cnt );
		editEntry( EditEntry );
		
		
		
	}
#if 0
	else if (msg == "pickAddresses(QCString,QCString,QStringList,...)" ) {
		QDataStream stream(data,IO_ReadOnly);
		QCString ch,m;
		QStringList types;
		stream >> ch >> m >> types;
		AddressPicker picker(abList,this,0,TRUE);
		picker.showMaximized();
		picker.setChoiceNames(types);
		int i=0;
		for (QStringList::ConstIterator it = types.begin(); it!=types.end(); ++it) {
			QStringList sel;
			stream >> sel;
			picker.setSelection(i++,sel);
		}
		picker.showMaximized();
		picker.exec();
		
		// ###### note: contacts may have been added - save here!
		
		setCentralWidget(abList);
		QCopEnvelope e(ch,m);
		i=0;
		for (QStringList::ConstIterator it = types.begin(); it!=types.end(); ++it) {
			QStringList sel = picker.selection(i++);
			e << sel;
		}
	}
#endif
	
}

void AddressbookWindow::editEntry( EntryMode entryMode )
{
	OContact entry;
	if ( !abEditor ) {
		abEditor = new ContactEditor( entry, &orderedFields, &slOrderedFields,
					      this, "editor" );
	}
	if ( entryMode == EditEntry )
		abEditor->setEntry( m_abView -> currentEntry() );
	else if ( entryMode == NewEntry )
		abEditor->setEntry( entry );
	// other things may chane the caption.
	abEditor->setCaption( tr("Edit Address") );
	
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
	abEditor->showMaximized();
#endif
	// fix the foxus...
	abEditor->setNameFocus();
	if ( abEditor->exec() ) {
		setFocus();
		if ( entryMode == NewEntry ) {
			OContact insertEntry = abEditor->entry();
			insertEntry.assignUid();
			m_abView -> addEntry( insertEntry );
		} else {
			OContact replEntry = abEditor->entry();

			if ( !replEntry.isValidUid() )
				replEntry.assignUid();

			m_abView -> replaceEntry( replEntry );
		}
	}
	populateCategories();

}

void AddressbookWindow::editPersonal()
{
	QString filename = addressbookPersonalVCardName();
	OContact me;
	if (QFile::exists(filename))
		me = OContact::readVCard( filename )[0];
	if ( !abEditor ) {
		qWarning("Editing personal data");
		abEditor = new ContactEditor( me, &orderedFields, &slOrderedFields,
					      this, "editor" );
	} else{
		abEditor->setEntry( me );
	}

	abEditor->setPersonalView( true );
	
	abEditor->setCaption(tr("Edit My Personal Details"));
	abEditor->showMaximized();
	
	// fix the foxus...
	abEditor->setNameFocus();
	if ( abEditor->exec() ) {
		setFocus();
		OContact new_personal = abEditor->entry();
		QString fname = addressbookPersonalVCardName();
		OContact::writeVCard( fname, new_personal );
		// Hier die persönliche Card zeigen..
// 		m_abView  -> init(new_personal);
// 		m_abView -> sync();
	}
	abEditor->setCaption( tr("Edit Address") );
	abEditor->setPersonalView( false );
}

void AddressbookWindow::slotPersonalView()
{
	if (!actionPersonal->isOn()) {
		// we just turned it off
		setCaption( tr("Contacts") );
		actionNew->setEnabled(TRUE);
		actionTrash->setEnabled(TRUE);
		actionFind->setEnabled(TRUE);
		slotUpdateToolbar(); // maybe some of the above could be moved there
		// :SX		showList();
		return;
	}
	
	// XXX need to disable some QActions.
	actionNew->setEnabled(FALSE);
	actionTrash->setEnabled(FALSE);
	actionFind->setEnabled(FALSE);
	actionMail->setEnabled(FALSE);
	
	setCaption( tr("Contacts - My Personal Details") );
	QString filename = addressbookPersonalVCardName();
	OContact me;
	if (QFile::exists(filename))
		me = OContact::readVCard( filename )[0];
	
	m_abView -> showContact ( me );

	// :SX
// 	listContainer->hide();
// 	m_abView() -> setCentralWidget( m_abView() );
// 	m_abView -> setView( CardView );
// 	m_abView ->setFocus();
}


void AddressbookWindow::listIsEmpty( bool empty )
{
	if ( !empty ) {
		deleteButton->setEnabled( TRUE );
	}
}

void AddressbookWindow::reload()
{
	syncing = FALSE;
	m_abView->clear();
	m_abView->reload();
}

void AddressbookWindow::flush()
{
	syncing = TRUE;
	m_abView->save();
}


void AddressbookWindow::closeEvent( QCloseEvent *e )
{
	
	if(syncing) {
		/* shouldn't we save, I hear you say? well its already been set
		   so that an edit can not occur during a sync, and we flushed
		   at the start of the sync, so there is no need to save
		   Saving however itself would cause problems. */
		e->accept();
		return;
	}
	//################## shouldn't always save
	// True, but the database handles this automatically ! (se)
	if ( save() )
		e->accept();
	else
		e->ignore();
}

/*
  Returns TRUE if it is OK to exit
*/

bool AddressbookWindow::save()
{
	if ( !m_abView->save() ) {
		if ( QMessageBox::critical( 0, tr( "Out of space" ),
					    tr("Unable to save information.\n"
					       "Free up some space\n"
					       "and try again.\n"
					       "\nQuit anyway?"),
					    QMessageBox::Yes|QMessageBox::Escape,
					    QMessageBox::No|QMessageBox::Default )
		     != QMessageBox::No )
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

#ifdef __DEBUG_RELEASE
void AddressbookWindow::slotSave()
{
	save();
}
#endif

// void AddressbookWindow::slotSettings()
// {
// 	AddressSettings frmSettings( this );
// #if defined(Q_WS_QWS) || defined(_WS_QWS_)
// 	frmSettings.showMaximized();
// #endif
	
// 	if ( frmSettings.exec() ) {
// 		allFields.clear();
// 		orderedFields.clear();
// 		slOrderedFields.clear();
// 		initFields();
// 		if ( abEditor )
// 			abEditor->loadFields();
// 		// abList->refresh();
// 	}
// }

// This should be moved to the contact editor.. (se)
void AddressbookWindow::initFields()
{
	// we really don't need the things from the configuration, anymore
	// only thing that is important are the important categories.  So,
	// Call the contact functions that correspond to these old functions...
	
	QStringList xmlFields = OContact::fields();
	QStringList visibleFields = OContact::untrfields();
	// QStringList trFields = OContact::trfields();

	xmlFields.remove( "Title" );
	visibleFields.remove( "Name Title" );
	visibleFields.remove( "Notes" );
	
	int i, version;
	Config cfg( "AddressBook" );
	QString zn;
	
	// ### Write a function to keep this from happening again...
	{
	QStringList::ConstIterator it;
	for ( i = 0, it = xmlFields.begin(); it != xmlFields.end(); ++it, i++ ) {
		allFields.append( i + 3 );
	}
	}
	
// 	cfg.setGroup( "Version" );
// 	version = cfg.readNumEntry( "version" );
// 	i = 0;

// 	if ( version >= ADDRESSVERSION ) {
		
// 		cfg.setGroup( "ImportantCategory" );
		
// 		zn = cfg.readEntry( "Category" + QString::number(i), QString::null );
// 		while ( !zn.isNull() ) {
// 			if ( zn.contains( "Work" ) || zn.contains( "Mb" ) ) {
// 				slOrderedFields.clear();
// 				break;
// 			}
// 			slOrderedFields.append( zn );
// 			zn = cfg.readEntry( "Category" + QString::number(++i), QString::null );
// 		}
		
		
// 	} else {
// 		QString str;
// 		str = getenv("HOME");
// 		str += "/Settings/AddressBook.conf";
// 		QFile::remove( str );
// 	}
	
// 	if ( slOrderedFields.count() > 0 ) {
// 		for( QStringList::ConstIterator it = slOrderedFields.begin();
// 		     it != slOrderedFields.end(); ++it ) {
// 			QValueList<int>::ConstIterator itVl;
// 			QStringList::ConstIterator itVis;
// 			itVl = allFields.begin();
// 			for ( itVis = visibleFields.begin();
// 			      itVis != visibleFields.end() && itVl != allFields.end();
// 			      ++itVis, ++itVl ) {
// 				if ( *it == *itVis && itVl != allFields.end() ) {
// 					orderedFields.append( *itVl );
// 				}
// 			}
// 		}
// 	} else {
		QValueList<int>::ConstIterator it;
		for ( it = allFields.begin(); it != allFields.end(); ++it )
			orderedFields.append( *it );
		
		slOrderedFields = visibleFields;
		orderedFields.remove( Qtopia::AddressUid );
		orderedFields.remove( Qtopia::Title );
		orderedFields.remove( Qtopia::Groups );
		orderedFields.remove( Qtopia::AddressCategory );
		orderedFields.remove( Qtopia::FirstName );
		orderedFields.remove( Qtopia::LastName );
		orderedFields.remove( Qtopia::DefaultEmail );
		orderedFields.remove( Qtopia::FileAs );
		orderedFields.remove( Qtopia::Notes );
		orderedFields.remove( Qtopia::Gender );
		slOrderedFields.remove( "Name Title" );
		slOrderedFields.remove( "First Name" );
		slOrderedFields.remove( "Last Name" );
		slOrderedFields.remove( "File As" );
		slOrderedFields.remove( "Default Email" );
		slOrderedFields.remove( "Notes" );
		slOrderedFields.remove( "Gender" );
		
// 	}
}


// AbLabel* AddressbookWindow::abView()
// {
// 	if ( !mView ) {
// 		mView = new AbLabel( this, "viewer" );
// 		mView->init( OContact()  );
// 		connect( mView, SIGNAL( okPressed() ), this, SLOT( slotListView() ) );
// 	}
// 	return mView;
// }


void AddressbookWindow::slotNotFound()
{
	qWarning("Got notfound signal !");
	QMessageBox::information( this, tr( "Not Found" ),
				  tr( "Unable to find a contact for this" ) + "\n"
				  + tr( "search pattern !" ) );

	
}
void AddressbookWindow::slotWrapAround()
{
	qWarning("Got wrap signal !");
// 	if ( doNotifyWrapAround )
// 		QMessageBox::information( this, tr( "End of list" ),
// 					  tr( "End of list. Wrap around now.. !" ) + "\n" );
		
}

void AddressbookWindow::slotSetCategory( int c )
{
	qWarning( "void AddressbookWindow::slotSetCategory( %d ) from %d", c, catMenu->count() );
	
	QString cat, book;
	AbView::Views  view = AbView::TableView;
	
	if ( c <= 0 )
		return;
	
	// Checkmark Book Menu Item Selected 
	if ( c < 3 )
		for ( unsigned int i = 1; i < 3; i++ )
			catMenu->setItemChecked( i, c == (int)i );
	// Checkmark Category Menu Item Selected 
	else
		for ( unsigned int i = 3; i < catMenu->count(); i++ )
			catMenu->setItemChecked( i, c == (int)i );
	
	for ( unsigned int i = 1; i < catMenu->count(); i++ ) {
		if (catMenu->isItemChecked( i )) {
			if ( i == 1 ){ // default List view
				book = QString::null;
				view = AbView::TableView;
// 			}else if ( i == 2 ){
// 				book = "Phone";
// 				view = AbView::PhoneBook;
// 			}else if ( i == 3 ){
// 				book = "Company";
// 				view = AbView::CompanyBook;
// 			}else if ( i == 4 ){
// 				book = "Email";
// 				view = AbView::EmailBook;
			}else if ( i == 2 ){
				book = "Cards";
				view = AbView::CardView;
			}else if ( i == 3 ) // default All Categories
				cat = QString::null;
			else if ( i == (unsigned int)catMenu->count() - 1 ){ // last menu option (seperator is counted, too) will be Unfiled
				cat = "Unfiled";
				qWarning ("Unfiled selected!!!");
			}else{
				cat = m_abView->categories()[i - 4];
			}
		}
	}
	
	m_abView -> setShowByCategory( view, cat );
	
	if ( book.isEmpty() )
		book = "List";
	if ( cat.isEmpty() )
		cat = "All";
	
	setCaption( tr( "Contacts" ) + " - " + tr( book ) + " - " + tr( cat ) );
}

void AddressbookWindow::slotViewSwitched( int view )
{
	qWarning( "void AddressbookWindow::slotViewSwitched( %d )", view );
	int menu = 0;
	switch ( view ){
	case AbView::TableView:
		menu = 1;
		break;
// 	case AbView::PhoneBook:
// 		menu = 2;
// 		break;
// 	case AbView::CompanyBook:
// 		menu = 3;
// 		break;
// 	case AbView::EmailBook:
// 		menu = 4;
// 		break;
	case AbView::CardView:
		menu = 2;
		break;
	}
	for ( unsigned int i = 1; i < 3; i++ ){
		if ( catMenu )
			catMenu->setItemChecked( i, menu == (int)i );
	}
}


void AddressbookWindow::slotSetLetter( char c ) {
	
 	m_abView->setShowByLetter( c );
	
}

void AddressbookWindow::populateCategories()
{
	catMenu->clear();
	
	int id, rememberId;
	id = 1;
	rememberId = 0;
	
	catMenu->insertItem( tr( "List" ), id++ );
// 	catMenu->insertItem( tr( "Phone Book" ), id++ );
// 	catMenu->insertItem( tr( "Company Book" ), id++ );
// 	catMenu->insertItem( tr( "Email Book" ), id++ );
	catMenu->insertItem( tr( "Cards" ), id++ );
	catMenu->insertSeparator();
	
	catMenu->insertItem( tr( "All" ), id++ );
	QStringList categories = m_abView->categories();
	categories.append( tr( "Unfiled" ) );
	for ( QStringList::Iterator it = categories.begin();
	      it != categories.end(); ++it ) {
		catMenu->insertItem( *it, id );
		if ( *it == m_abView -> showCategory() )
			rememberId = id;
		++id;
	}

	// :SX
// 	if ( abList->showBook().isEmpty() ) {
// 		catMenu->setItemChecked( 1, true );
// 	} else if ( abList->showBook() == "Phone" ) {
// 		catMenu->setItemChecked( 2, true );
// 	} else if ( abList->showBook() == "Company" ) {
// 		catMenu->setItemChecked( 3, true );
// 	} else if ( abList->showBook() == "Email" ) {
// 		catMenu->setItemChecked( 4, true );
// 	} else if ( abList->showBook() == "Cards" ) {
// 		catMenu->setItemChecked( 5, true );
// 	}
	
	if ( m_abView -> showCategory().isEmpty() ) {
		slotSetCategory( 3 );
	}
	else {
		slotSetCategory( rememberId );
	}
}

