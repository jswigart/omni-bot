#include <QObject>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QGraphicsView>

#ifndef __MAPWIDGET_H__
#define __MAPWIDGET_H__

namespace RemoteLib
{
	class DataBuffer;
};

class MapWidget : public QGraphicsView {
	Q_OBJECT
public:
	MapWidget( QWidget *parent = 0 );

	bool msgCircle( RemoteLib::DataBuffer & db );
	bool msgLine( RemoteLib::DataBuffer & db );
	bool msgImage( RemoteLib::DataBuffer & db );
protected:
	void resizeEvent( QResizeEvent * event );
	void dragEnterEvent( QDragEnterEvent *event );
	void dragLeaveEvent( QDragLeaveEvent * event );
	void dragMoveEvent ( QDragMoveEvent * event );
	void dropEvent( QDropEvent *event );

	void wheelEvent( QWheelEvent * event );

	void mousePressEvent( QMouseEvent *event );
private:
	QGraphicsScene			world;

	QGraphicsItemGroup *	rootGroup;

	QGraphicsItemGroup * findGroupForPath( const QString & path );
	QGraphicsItem * findItemInGroup( const QString & path, QString & name, QGraphicsItemGroup * group );
};

#endif
