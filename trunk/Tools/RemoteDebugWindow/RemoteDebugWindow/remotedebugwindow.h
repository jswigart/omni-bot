#ifndef REMOTEDEBUGWINDOW_H
#define REMOTEDEBUGWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_remotedebugwindow.h"
#include <QTcpSocket>
#include <QSystemTrayIcon>

class QStandardItemModel;
class QDeclarativeComponent;

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

	QDeclarativeComponent *	qmlEntityComponent;
	QDeclarativeContext *	mainContext;

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

	void updateEntity( RemoteLib::DataBuffer & db );

	QGraphicsObject * entityFromHandle( int handle );
private slots:
	void processMessages();

	bool msgConfigName( RemoteLib::DataBuffer & db );
	bool msgTreeNode( RemoteLib::DataBuffer & db );

	// actions
	void onConnectLocalHost();

	// network
	void socketConnected();
	void socketDisconnected();
	void socketReadyToRead();
	void socketDisplayError(QAbstractSocket::SocketError socketError);
};

#endif // REMOTEDEBUGWINDOW_H
