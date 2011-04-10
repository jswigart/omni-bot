#include <QtGui>
#include <QtNetwork>

#include <QDeclarativeProperty>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeItem>


#include "dataBuffer.h"
#include "messageTags.h"
#include "socket.h"

#include "remotedebugwindow.h"

enum UserDataKeys { EntityHandle = 1 };

//////////////////////////////////////////////////////////////////////////

enum ColumnTypes {
	Header,
	DebugStr,
	NumColumns
};

RemoteDebugWindow::RemoteDebugWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, trayIconDisconnected( ":/RemoteDebugWindow/Resources/disconnected.png" )
	, trayIconConnected( ":/RemoteDebugWindow/Resources/connected.png" )
{
	ui.setupUi(this);

	setupActions();

	createTrayIcon();

	setupNetwork();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(processMessages()));
	timer->start(50);

	model = new QStandardItemModel( 0,0 );
	model->insertColumns( 0, NumColumns );
	ui.stateTree->setModel( model );

	//////////////////////////////////////////////////////////////////////////
	//ui.declarativeView->engine()->setBaseUrl(QUrl::fromLocalFile("./qml/"));
	ui.declarativeView->setSource( QUrl::fromLocalFile( "./qml/main.qml" ) );
	while( ui.declarativeView->status() != QDeclarativeView::Ready ) {
	}

	QDeclarativeEngine * engine = ui.declarativeView->engine();
	mainContext = engine->contextForObject(ui.declarativeView->rootObject());
	
	qmlEntityComponent = new QDeclarativeComponent(engine,ui.declarativeView->rootObject());
	qmlEntityComponent->loadUrl(QUrl::fromLocalFile("./qml/entity.qml"));

	RemoteLib::DataBuffer db( 1024 );
	db.beginWrite( RemoteLib::DataBuffer::WriteModeAllOrNone );
	db.startSizeHeader();
	db.writeInt32( RemoteLib::ID_qmlEntity );

	db.writeInt32( 1 ); // entity handle

	db.writeSmallString( "name" );
	db.writeSmallString( "Entity 1" );

	db.writeSmallString( "x" );
	db.writeFloat32( 100.0f );
	db.writeSmallString( "y" );
	db.writeFloat32( 100.0f );

	db.writeSmallString( "yaw" );
	db.writeFloat32( 45 );

	db.writeSmallString( "classid" );
	db.writeInt32( 3 );

	db.endSizeHeader();
	db.endWrite();

	//////////////////////////////////////////////////////////////////////////

	db.beginRead( RemoteLib::DataBuffer::ReadModeAllOrNone );

	int32 tagId = 0, blockSize = 0;
	db.readInt32( blockSize );
	db.readInt32( tagId );
	Q_ASSERT_X( tagId == RemoteLib::ID_qmlEntity, __FUNCTION__, "no" );
	updateEntity( db );
	/*createPlayer( 1, "Entity 1", 50, 50, 1 );
	createPlayer( 2, "Entity 2", 100, 100, 2 );	
	createPlayer( 3, "Entity 3", 150, 150, 3 );
	createPlayer( 4, "Entity 4", 200, 200, 4 );
	createPlayer( 5, "Entity 5", 250, 250, 5 );*/
}

RemoteDebugWindow::~RemoteDebugWindow() {
}

void RemoteDebugWindow::writeSettings()
{
	QSettings settings("settings.ini", QSettings::IniFormat);

	settings.beginGroup("LayOut");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	settings.setValue("style", QApplication::style()->objectName());
	settings.setValue("font", QApplication::font().toString());		
	settings.endGroup();
}

void RemoteDebugWindow::readSettings()
{
	QSettings settings("settings.ini", QSettings::IniFormat);

	settings.beginGroup("LayOut");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
	QApplication::setStyle( settings.value("style",QApplication::style()->objectName()).toString() );

	QFont savedFont = QApplication::font();
	if ( savedFont.fromString( settings.value("font").toString() ) ) {
		QApplication::setFont( savedFont );
	}
	settings.endGroup();
}

void RemoteDebugWindow::closeEvent(QCloseEvent *event) {
	if(tcpSocket->state() != QAbstractSocket::UnconnectedState)
	{
		QMessageBox msgBox(this);
		msgBox.setText("You are connected to a remote session.");
		msgBox.setInformativeText("Do you really want to close the viewer?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		switch( msgBox.exec() )
		{
		case QMessageBox::Yes:
			writeSettings();
			event->accept();
			tcpSocket->abort();
			return;
		case QMessageBox::No:
			event->ignore();
			return;
		}
	}

	writeSettings();
	event->accept();
}

void RemoteDebugWindow::createTrayIcon() {
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(ui.actionExit);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon( trayIconDisconnected );
	trayIcon->show();
}

void RemoteDebugWindow::iconActivated( QSystemTrayIcon::ActivationReason reason ) {
	switch (reason) {
	 case QSystemTrayIcon::Trigger:
	 case QSystemTrayIcon::DoubleClick:		 
		 break;
	 case QSystemTrayIcon::MiddleClick:
		 break;
	 default:
		 ;
	}
}

void RemoteDebugWindow::setupActions() {
	connect(ui.actionConnect_LocalHost, SIGNAL(triggered(bool)), this, SLOT(onConnectLocalHost()));
}

void RemoteDebugWindow::onConnectLocalHost() {
	QString port = QString::number( RemoteLib::TcpSocket::DefaultPort );
	tcpSocket->abort();
	tcpSocket->connectToHost(QHostAddress::LocalHost,port.toInt());
}

void RemoteDebugWindow::setupNetwork() {
	// create network socket and slots
	tcpSocket = new QTcpSocket(this);
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(socketReadyToRead()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(socketDisplayError(QAbstractSocket::SocketError)));
}

void RemoteDebugWindow::socketConnected() {
	/*ui.actionConnect->setEnabled(false);
	ui.actionDisconnect->setEnabled(true);*/

	trayIcon->setIcon( trayIconConnected );
	statusBar()->showMessage("Connected...", 2000);
}

void RemoteDebugWindow::socketDisconnected() {
	trayIcon->setIcon( trayIconDisconnected );
	/*ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);*/
}

void RemoteDebugWindow::socketReadyToRead() {
	QDataStream in(tcpSocket);
	const int sizeBefore = recvdData.size();
	recvdData.append(tcpSocket->readAll());
	const int sizeAfter = recvdData.size();
	processMessages();
}

void RemoteDebugWindow::socketDisplayError(QAbstractSocket::SocketError socketError) {
	switch ( socketError ) {
	case QAbstractSocket::RemoteHostClosedError:
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information(this, tr("Remote Viewer"),
			tr("The host was not found. Please check the "
			"host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr("Remote Viewer"),
			tr("The connection was refused by the peer. "
			"Make sure the remote application is running, "
			"and check that the host name and port "
			"settings are correct."));
		break;
	default:
		QMessageBox::information(this, tr("Remote Viewer"),
			tr("The following error occurred: %1.")
			.arg(tcpSocket->errorString()));
	}
}

QModelIndex RemoteDebugWindow::findNodeForPath( const QString & path ) {
	QModelIndexList nodeMatches = model->match( 
		model->index(0,0), 
		Qt::UserRole, 
		path, 
		1, 
		Qt::MatchExactly | Qt::MatchRecursive );

	if ( !nodeMatches.isEmpty() ) {
		return nodeMatches.first();
	} else {
		QModelIndex parent;

		QString nodeName = path;
		const int lastSlash = path.lastIndexOf( "/" );
		if ( lastSlash != -1 ) {
			QString parentPath = path.leftRef( lastSlash ).toString();
			nodeName = path.midRef( lastSlash+1 ).toString();
			parent = findNodeForPath( parentPath );
		} else {
			ui.debugWidgets->insertTopLevelItem( 
				ui.debugWidgets->topLevelItemCount(),
				new QTreeWidgetItem( (QTreeWidgetItem*)0, QStringList( QString( path ) ) ) );
		}

		const int row = model->rowCount( parent );
		model->insertRow( row, parent );
		//model->insertColumns( 0, NumColumns, parent );
		QModelIndex c0 = model->index( row, 0, parent );
		QModelIndex c1 = model->index( row, 1, parent );
		model->itemFromIndex( c0 )->setColumnCount( NumColumns );
		model->itemFromIndex( c1 );
		model->setData( c0, nodeName, Qt::DisplayRole );
		model->setData( c0, path, Qt::UserRole );
		//model->setData( c1, "col1", Qt::UserRole );

		ui.stateTree->setExpanded( c0, true );

		return c0;
	}
}

void RemoteDebugWindow::processMessages() {
	RemoteLib::DataBuffer db( recvdData.data(), 
		recvdData.size(),
		RemoteLib::DataBuffer::ModeRead );

	db.setAssertOnError();
	while( db.getReadBytesLeft() >= 4 ) {
		const uint32 baseBytes = db.getBytesRead();
		db.beginRead( RemoteLib::DataBuffer::ReadModeAllOrNone );

		int32 tagId = 0, blockSize = 0;
		db.readInt32( blockSize );

		if ( !db.canRead( blockSize ) ) {
			db.endRead( true );
			break;
		}

		db.readInt32( tagId );
		
		switch( tagId )
		{
		case RemoteLib::ID_configName:
			{
				msgConfigName( db );
				break;
			}
		case RemoteLib::ID_ack:
			{
				int sz = 4;
				// keepalive
				tcpSocket->write( (char*)&sz, 4 );
				tcpSocket->write( (char*)&tagId, 4 );
				break;
			}
		case RemoteLib::ID_delete:
			{
				enum { BufferSz = 256 };
				char groupBuf[ BufferSz ] = {};
				char nameBuf[ BufferSz ] = {};

				QString group, name;
				db.readSmallString( groupBuf, BufferSz ); group = groupBuf;
				db.readSmallString( nameBuf, BufferSz ); name = nameBuf;
				ui.graphics2d->msgDelete( group, name );
				break;
			}
		case RemoteLib::ID_treeNode:
			{
				msgTreeNode( db );
				break;
			}
		case RemoteLib::ID_circle:
			{
				ui.graphics2d->msgCircle( db );
				break;
			}
		case RemoteLib::ID_line:
			{
				ui.graphics2d->msgLine( db );
				break;
			}
		case RemoteLib::ID_obb:
			{
				ui.graphics2d->msgRect( db );
				break;
			}
		case RemoteLib::ID_image:
			{
				ui.graphics2d->msgImage( db );
				break;
			}
		case RemoteLib::ID_token:
			{
				ui.graphics2d->msgToken( db );
				break;
			}
		case RemoteLib::ID_qmlEntity:
			{
				updateEntity( db );				
				break;
			}
		default:
			Q_ASSERT_X( 0, __FUNCTION__, "unhandled message type" );
			break;
		}

		const uint32 blockBytesRead = db.getBytesRead() - baseBytes;
		Q_ASSERT_X( blockBytesRead == blockSize+4, __FUNCTION__, "databuffer read error" );
		db.endRead();
	}
	recvdData.remove( 0, db.getBytesRead() );
}

bool RemoteDebugWindow::msgConfigName( RemoteLib::DataBuffer & db ) {
	QString configName;

	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};
	db.readString( buffer, BufferSz ); configName = buffer;

	QSettings settings("settings.ini", QSettings::IniFormat);

	settings.beginGroup( configName );
	const float viewScaleX = settings.value( "scaleX", QVariant( 1.0f ) ).toFloat();
	const float viewScaleY = settings.value( "scaleY", QVariant( 1.0f ) ).toFloat();
	ui.graphics2d->scale( viewScaleX, viewScaleY );
	
	settings.endGroup();

	return !db.hasReadError();
}

bool RemoteDebugWindow::msgTreeNode( RemoteLib::DataBuffer & db ) {
	int8 r = 0, g = 0, b = 0, a = 0;
	QString path, name, dbgInfo;
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	db.readString( buffer, BufferSz ); path = buffer;
	db.readString( buffer, BufferSz ); name = buffer;
	db.readString( buffer, BufferSz ); dbgInfo = buffer;
	db.readInt8( r );
	db.readInt8( g );
	db.readInt8( b );
	db.readInt8( a );

	if ( !db.hasReadError() ) {
		QModelIndex n0 = findNodeForPath( path );
		QModelIndex n1 = model->index( n0.row(), 1, n0.parent() );
		model->setData( n0, name, Qt::DisplayRole);
		model->setData( n1, dbgInfo, Qt::DisplayRole);
		model->setData( n0, QColor( qRgba( r, g, b, a ) ), Qt::TextColorRole );
	} else {
		Q_ASSERT_X( !db.hasReadError(), __FUNCTION__, "databuffer read error" );
	}
	return !db.hasReadError();
}

//////////////////////////////////////////////////////////////////////////

QGraphicsObject * RemoteDebugWindow::entityFromHandle( int handle ) {
	QDeclarativeItem * rootItem = qobject_cast<QDeclarativeItem*>(ui.declarativeView->rootObject());
	const QObjectList & children = rootItem->children();
	for ( QObjectList::const_iterator it = rootItem->children().begin();
		it != rootItem->children().end(); 
		++it ) {
			const QGraphicsObject * item = qobject_cast<const QGraphicsObject*>((*it));
			if ( item && item->data( EntityHandle ) == handle ) {
				return const_cast<QGraphicsObject *>( item );
			}
	}
	return NULL;
}

void RemoteDebugWindow::updateEntity( RemoteLib::DataBuffer & db ) {
	// there should ALWAYS be an entity handle
	int32 entityHandle = 0;
	if(!db.readInt32(entityHandle)) {
		return;
	}

	QGraphicsObject * entity = entityFromHandle( entityHandle );
	if ( !entity ) {
		QDeclarativeItem * rootItem = qobject_cast<QDeclarativeItem*>(ui.declarativeView->rootObject());
		QDeclarativeItem * item = qobject_cast<QDeclarativeItem*>(qmlEntityComponent->create(mainContext));
		item->setParentItem( rootItem );
		item->setData(EntityHandle,entityHandle);
		entity = item;
	}

	enum { MaxPropertySize = 256 };
	char propertyName[ MaxPropertySize ];
	while( db.readSmallString( propertyName, MaxPropertySize ) ) {
		QVariant prop = entity->property( propertyName );
		if ( prop.isNull() ) {
			Q_ASSERT_X( 0, __FUNCTION__, "unknown property" );
		} else {
			QVariant::Type propType = prop.type();
			switch( propType )
			{
			case QVariant::Int:
				{
					int32 val = 0.0f;
					if ( db.read32( val ) ) {
						if ( prop != val ) {
							entity->setProperty( propertyName, val );
						}
					}
					break;
				}
			case QVariant::String:
				{
					enum { MaxPropertySize = 256 };
					char val[ MaxPropertySize ];
					if ( db.readSmallString( val, MaxPropertySize ) ) {
						if ( prop != val ) {
							entity->setProperty( propertyName, val );
						}
					}
					break;
				}
			case QMetaType::Float:
			case QMetaType::Double:
				{
					float val = 0.0f;
					if ( db.readFloat32( val ) ) {
						if ( prop != val ) {
							entity->setProperty( propertyName, val );
						}
					}
					break;
				}
			default:
				{
					QString propTypeName = prop.typeName();
					Q_ASSERT_X( 0, __FUNCTION__, "unhandled property type" );
				}
			}
		}
	}
}
