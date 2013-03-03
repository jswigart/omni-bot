#include <QtGui>
#include <QtNetwork>

#include <QDeclarativeProperty>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeItem>
#include <QGLWidget>

#include "dataBuffer.h"
#include "messageTags.h"
#include "socket.h"

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"

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
	, objComponents( NULL )
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

	//QDeclarativeEngine * engine = ui.declarativeMap->engine();
	QDeclarativeEngine * engine = new QDeclarativeEngine( ui.declarativeMap );

	QGLWidget * glWidget = new QGLWidget;
	glWidget->qglClearColor( QColor( "cyan" ) );
	ui.declarativeMap->setViewport( glWidget ); 
	InitView( ui.declarativeMap, QUrl::fromLocalFile( "./qml/common/mapView.qml" ) );

	objViewPort = ui.declarativeMap->rootObject();
	objViewPort = objViewPort->findChild<QObject*>( "viewport" );
	if ( objViewPort == NULL ) {
		QMessageBox msgBox(this);
		msgBox.setText("Qml Error.");
		msgBox.setInformativeText( "Expected 'viewport' for 3d content." );
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
	}
	
	mainContext = engine->contextForObject(objViewPort);
	
	objComponents = new QObject();

	//CacheComponent( engine, qmlEntityComponent, "./qml/common/entity.qml");

	qDebug() << "Binary Loc " << QLibraryInfo::location( QLibraryInfo::BinariesPath );
	qDebug() << "Plugin Loc " << QLibraryInfo::location( QLibraryInfo::PluginsPath );
	qDebug() << "Import Loc " << QLibraryInfo::location( QLibraryInfo::ImportsPath );
}

RemoteDebugWindow::~RemoteDebugWindow() {
	delete objComponents;
}

void RemoteDebugWindow::InitView( QDeclarativeView *view, const QUrl & file ) {
	view->setSource( file );

	if ( view->status() == QDeclarativeView::Error ) {
		qDebug() << view->errors();

		if ( view->errors().count() > 0 ) {
			QMessageBox msgBox(this);
			msgBox.setText("Qml Error.");
			msgBox.setInformativeText( view->errors()[ 0 ].toString() );
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.exec();
		}
		return;
	}

	while( view->status() != QDeclarativeView::Ready ) {
	}
}

void RemoteDebugWindow::CacheComponent(QDeclarativeEngine *engine, QDeclarativeComponent *& component, const QString & file ) {
	component = new QDeclarativeComponent(engine,objViewPort);
	component->loadUrl(QUrl::fromLocalFile(file));

	if ( component->errors().count() > 0 ) {
		qDebug() << component->errors();

		QMessageBox msgBox(this);
		msgBox.setText("Component Not Found.");
		msgBox.setInformativeText( component->errorString() );
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();

		delete component;
		component = NULL;
	}
}

QDeclarativeComponent * RemoteDebugWindow::FindComponentType( const QString & componentName ) {
	QDeclarativeComponent *component = objComponents->findChild<QDeclarativeComponent*>( componentName );
	if ( !component ) {
		QString componentFile;
		QTextStream stream( &componentFile );
		stream << "./qml/common/" << componentName << ".qml";
		
		CacheComponent( ui.declarativeMap->engine(), component, componentFile );
		if ( component ) {
			component->setObjectName( componentName );
			component->setParent( objComponents );
		}
	}
	return component;
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

	// todo: delete all subobjects
}

void RemoteDebugWindow::socketReadyToRead() {
	QDataStream in(tcpSocket);
	const int sizeBefore = recvdData.size();
	recvdData.append(tcpSocket->readAll());
	const int sizeAfter = recvdData.size();
	processMessages();

	qDebug() << "Read" << (sizeAfter - sizeBefore) << "bytes\n";
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
	if ( recvdData.isEmpty() ) {
		return;
	}

	using namespace google::protobuf;
	io::CodedInputStream stream( (google::protobuf::uint8*)recvdData.data(), recvdData.size() );

	google::protobuf::uint32 msgSize = 0, bytesRead = 0;
	while( stream.ReadVarint32( &msgSize ) && msgSize > 0 ) {
		if ( stream.BytesUntilLimit() >= msgSize ) {
			const io::CodedInputStream::Limit lastLimit = stream.PushLimit( msgSize );

			Remote::Packet packet;
			if ( packet.ParseFromCodedStream( &stream ) ) {
				switch( packet.payloadformat() ) {
					case Remote::PACKET_GAME:
						{
							Remote::Game msg;

							bool parsed = false;
							if ( packet.payloadformat() == Remote::PAYLOAD_TEXT ) {
								parsed = TextFormat::ParseFromString( packet.payloaddata(), &msg );
							} else {
								parsed = msg.ParseFromString( packet.payloaddata() );
							}

							if ( parsed ) {
								//ReflectMessageToQML( entity );
							} else {
								qDebug() << "Error Parsing Game Message\n";
							}
							break;
						}
					case Remote::PACKET_ENTITY:
						{
							Remote::Entity msg;

							bool parsed = false;
							if ( packet.payloadformat() == Remote::PAYLOAD_TEXT ) {
								parsed = TextFormat::ParseFromString( packet.payloaddata(), &msg );
							} else {
								parsed = msg.ParseFromString( packet.payloaddata() );
							}

							if ( parsed ) {
								ReflectMessageToQML( msg );
							} else {
								qDebug() << "Error Parsing Entity Message\n";
							}
							break;
						}
				}	
			} else {
				qDebug() << "Error Parsing Packet\n";
			}
			stream.PopLimit( lastLimit );

			bytesRead += msgSize + io::CodedOutputStream::VarintSize32( msgSize );
		} else {
			// the complete message is not in the buffer
			break;
		}
	}

	// remove all we could process at this time
	recvdData.remove( 0, bytesRead );
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
	//ui.graphics2d->scale( viewScaleX, viewScaleY );
	
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

bool RemoteDebugWindow::msgScriptPrint( RemoteLib::DataBuffer & db ) {
	QString msg;
	enum { BufferSz = 2048 };
	char buffer[ BufferSz ] = {};

	int32 rgba = 0;
	db.readInt32( rgba );
	db.readString( buffer, BufferSz ); msg = buffer;

	QString html;
	QTextStream stream( &html );
	stream << buffer;
	
	ui.scriptOutput->append( html );
	return !db.hasReadError();
}

//////////////////////////////////////////////////////////////////////////

QGraphicsObject * RemoteDebugWindow::entityFromHandle( int handle ) {
	QDeclarativeItem * rootItem = qobject_cast<QDeclarativeItem*>(objViewPort);
	const QObjectList & children = rootItem->children();
	for ( QObjectList::const_iterator it = rootItem->children().begin();
		it != rootItem->children().end(); 
		++it ) {
			const QGraphicsObject * item = qobject_cast<const QGraphicsObject*>((*it));
			QVariant handleData = item ? item->data( EntityHandle ) : QVariant();
			if ( handleData.toInt() == handle ) {
				return const_cast<QGraphicsObject *>( item );
			}
	}
	return NULL;
}

void RemoteDebugWindow::deleteEntity( int entityHandle ) {
	QGraphicsObject * entity = entityFromHandle( entityHandle );
	delete entity;
}

void RemoteDebugWindow::ReflectMessageToQML( const Remote::Entity & msg ) {
	
	QDeclarativeComponent * component = FindComponentType( "entity" );
	if ( component == NULL ) {
		QMessageBox msgBox(this);
		msgBox.setText("Unknown Component type, disconnecting...");
		msgBox.setInformativeText( QString("Component Type Not Found: ") + "entity" );
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();

		tcpSocket->disconnect();
		return;
	}

	bool finishEntity = false;

	QGraphicsObject * entity = entityFromHandle( msg.uid() );
	if ( !entity ) {
		QDeclarativeItem * rootItem = qobject_cast<QDeclarativeItem*>(objViewPort);
		QDeclarativeItem * item = qobject_cast<QDeclarativeItem*>(component->beginCreate(mainContext));
		item->setParentItem( rootItem );
		item->setParent( rootItem );
		item->setData( EntityHandle, msg.uid() );
		entity = item;

		entity->setProperty( "handle", msg.uid() );

		finishEntity = true;
	}

	if ( msg.position().has_x() ) {
		entity->setProperty( "x", msg.position().x() );
	}
	if ( msg.position().has_y() ) {
		entity->setProperty( "y", msg.position().y() );
	}
	if ( msg.position().has_z() ) {
		entity->setProperty( "z", msg.position().z() );
	}

	using namespace google::protobuf;
	const Reflection * reflection = msg.GetReflection();

	typedef std::vector<const FieldDescriptor*> FieldDescList;
	FieldDescList descriptors;
	reflection->ListFields( msg, &descriptors );
	for ( int i = 0; i < descriptors.size(); ++i ) {
		std::string fieldName = descriptors[ i ]->name();

		switch( descriptors[ i ]->type() ) {
			case FieldDescriptor::TYPE_DOUBLE:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetDouble( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_FLOAT:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetFloat( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_INT64:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetInt64( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_UINT64:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetUInt64( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_INT32:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetInt32( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_FIXED64:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetUInt64( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_FIXED32:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetUInt32( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_BOOL:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetBool( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_STRING:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetString( msg, descriptors[ i ] ).c_str() );
					break;
				}
			case FieldDescriptor::TYPE_GROUP:
				{
					// todo
					break;
				}
			case FieldDescriptor::TYPE_MESSAGE:
				{
					const Message & subMsg = reflection->GetMessage( msg, descriptors[ i ] );
					const Reflection * subMsgReflection = subMsg.GetReflection();
					
					/*QVariant currentProp = entity->property( fieldName.c_str() );
					if ( currentProp.type() == QVariant::Vector3D ) {
						subMsgReflection->flo
					}*/

					
					
					FieldDescList subMsgFields;
					subMsgReflection->ListFields( subMsg, &subMsgFields );

					// todo
					break;
				}
			case FieldDescriptor::TYPE_BYTES:
				{
					const std::string byteString = reflection->GetString( msg, descriptors[ i ] );
					entity->setProperty( fieldName.c_str(), QByteArray( byteString.c_str(), byteString.size() ) );
					break;
				}
			case FieldDescriptor::TYPE_UINT32:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetUInt32( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_ENUM:
				{
					const EnumValueDescriptor * enumval = reflection->GetEnum( msg, descriptors[ i ] );
					entity->setProperty( fieldName.c_str(), enumval->name().c_str() );
					break;
				}
			case FieldDescriptor::TYPE_SFIXED32:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetInt32( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_SFIXED64:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetInt64( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_SINT32:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetInt32( msg, descriptors[ i ] ) );
					break;
				}
			case FieldDescriptor::TYPE_SINT64:
				{
					entity->setProperty( fieldName.c_str(), reflection->GetInt64( msg, descriptors[ i ] ) );
					break;
				}
		}
	}

	if ( finishEntity ) {
		component->completeCreate();
	}
}
