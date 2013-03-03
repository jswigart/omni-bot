#ifndef REMOTEDEBUGWINDOW_H
#define REMOTEDEBUGWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_remotedebugwindow.h"
#include <QList>
#include <QTcpSocket>
#include <QSystemTrayIcon>

#include "common.pb.h"

class QStandardItemModel;
class QDeclarativeComponent;

namespace RemoteLib {
	class DataBuffer;
};

class RemoteDebugWindow : public QMainWindow
{
	Q_OBJECT
public:
	RemoteDebugWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~RemoteDebugWindow();

	void closeEvent(QCloseEvent *event);
private:
	Ui::RemoteDebugWindowClass ui;

	void writeSettings();
	void readSettings();

	void setupActions();

	QDeclarativeContext *	mainContext;
	QObject *				objViewPort;
	QObject *				objComponents;

	// tray icon
	QSystemTrayIcon *	trayIcon;
	QMenu *				trayIconMenu;
	QIcon				trayIconDisconnected;
	QIcon				trayIconConnected;
	void createTrayIcon();
	void iconActivated( QSystemTrayIcon::ActivationReason reason );

	// network
	QTcpSocket *		tcpSocket;
	QByteArray			recvdData;
	QByteArray			sendData;
	void setupNetwork();

	QStandardItemModel * model;

	QModelIndex findNodeForPath( const QString & path );

	void InitView( QDeclarativeView *view, const QUrl & file );
	void CacheComponent(QDeclarativeEngine *engine, QDeclarativeComponent *& component, const QString & file );

	void deleteEntity( int entityHandle );
	void ReflectMessageToQML( const Remote::Entity & msg );

	QGraphicsObject * entityFromHandle( int handle );
private slots:
	void processMessages();

	bool msgConfigName( RemoteLib::DataBuffer & db );
	bool msgTreeNode( RemoteLib::DataBuffer & db );
	bool msgScriptPrint( RemoteLib::DataBuffer & db );

	QDeclarativeComponent * FindComponentType( const QString & componentName );

	// actions
	void onConnectLocalHost();

	// network
	void socketConnected();
	void socketDisconnected();
	void socketReadyToRead();
	void socketDisplayError(QAbstractSocket::SocketError socketError);
};

#endif // REMOTEDEBUGWINDOW_H
