#include <QObject>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QGraphicsView>

#ifndef __MAPWIDGET_H__
#define __MAPWIDGET_H__

namespace RemoteLib { class DataBuffer; };

//////////////////////////////////////////////////////////////////////////

class MapScene : public QGraphicsScene {
	Q_OBJECT
public:
	MapScene();
	void contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent );
	//void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
	//void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event );
	//void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
	//void drawBackground ( QPainter * painter, const QRectF & rect );
	//void drawForeground ( QPainter * painter, const QRectF & rect );
	//void drawItems ( QPainter * painter, int numItems, QGraphicsItem *[] items, const QStyleOptionGraphicsItem[] options, QWidget * widget = 0 );
	//void dropEvent ( QGraphicsSceneDragDropEvent * event );
	//void focusInEvent ( QFocusEvent * focusEvent );
	//void focusOutEvent ( QFocusEvent * focusEvent );
	//void helpEvent ( QGraphicsSceneHelpEvent * helpEvent );
	//void inputMethodEvent ( QInputMethodEvent * event );
	void keyPressEvent ( QKeyEvent * keyEvent );
	void keyReleaseEvent ( QKeyEvent * keyEvent );
	void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	//void wheelEvent ( QGraphicsSceneWheelEvent * wheelEvent );

	
};

//////////////////////////////////////////////////////////////////////////

class MapWidget : public QGraphicsView {
	Q_OBJECT
public:
	MapWidget( QWidget *parent = 0 );

	bool msgCircle( RemoteLib::DataBuffer & db );
	bool msgLine( RemoteLib::DataBuffer & db );
	bool msgObb( RemoteLib::DataBuffer & db );
	bool msgImage( RemoteLib::DataBuffer & db );
protected:
	void resizeEvent( QResizeEvent * event );
	void dragEnterEvent( QDragEnterEvent *event );
	void dragLeaveEvent( QDragLeaveEvent * event );
	void dragMoveEvent ( QDragMoveEvent * event );
	void dropEvent( QDropEvent *event );

	void wheelEvent( QWheelEvent * event );
private:
	MapScene				world;

	QPointF					sceneLeftClickPos;

	QGraphicsItemGroup *	rootGroup;

	QGraphicsItemGroup * findGroupForPath( const QString & path );
	QGraphicsItem * findItemInGroup( const QString & path, QString & name, QGraphicsItemGroup * group );
};

#endif
