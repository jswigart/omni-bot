#include <QtGui>
#include <QtNetwork>

#include "dataBuffer.h"
#include "messageTags.h"
#include "socket.h"

#include "remotedebugwindow.h"

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
		msgBox.setText("You are connected to a debug session.");
		msgBox.setInformativeText("Do you really want to close the debugger?");
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
	QString ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	QString port = QString::number( RemoteLib::TcpSocket::DefaultPort );
	tcpSocket->abort();
	tcpSocket->connectToHost(ipAddress,port.toInt());
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
		QMessageBox::information(this, tr("GM Debugger"),
			tr("The host was not found. Please check the "
			"host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr("GM Debugger"),
			tr("The connection was refused by the peer. "
			"Make sure the GM application is running, "
			"and check that the host name and port "
			"settings are correct."));
		break;
	default:
		QMessageBox::information(this, tr("GM Debugger"),
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

	const char * lastTagProcessed = "";

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
		case RemoteLib::ID_ack:
			{
				lastTagProcessed = "ack";
				int sz = 4;
				// keepalive
				tcpSocket->write( (char*)&sz, 4 );
				tcpSocket->write( (char*)&tagId, 4 );
				break;
			}
		case RemoteLib::ID_treeNode:
			{
				lastTagProcessed = "treeNode";
				msgTreeNode( db );
				break;
			}
		case RemoteLib::ID_circle:
			{
				lastTagProcessed = "circle";
				ui.graphics2d->msgCircle( db );
				break;
			}
		case RemoteLib::ID_line:
			{
				lastTagProcessed = "line";
				ui.graphics2d->msgLine( db );
				break;
			}
		case RemoteLib::ID_image:
			{
				lastTagProcessed = "image";
				ui.graphics2d->msgImage( db );
				break;
			}
		}

		const uint32 blockBytesRead = db.getBytesRead() - baseBytes;
		Q_ASSERT_X( blockBytesRead == blockSize+4, __FUNCTION__, "databuffer read error" );
		db.endRead();
	}
	recvdData.remove( 0, db.getBytesRead() );
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
