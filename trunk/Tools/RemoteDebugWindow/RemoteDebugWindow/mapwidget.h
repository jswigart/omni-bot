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

	bool msgDelete( const QString & group, const QString & name );
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

	void splitPath( const QString & fullPath, QString & subPath, QString & name );

	QGraphicsItem * findItemInGroup( const QString & name, QGraphicsItemGroup * group );
	QGraphicsItemGroup * findGroup( const QString & path, QGraphicsItemGroup * group = NULL );

	enum UserDataKeys { PathKey = 1, TagName = 2 };

	template< typename T >
	T *	findItem( const QString & tag, QGraphicsItemGroup * group ) {
		Q_ASSERT_X( group, __FUNCTION__, "no group provided!" );

		QList<QGraphicsItem *> & children = group->children();
		for ( QList<QGraphicsItem *>::iterator it = children.begin();
			it != children.end(); 
			++it ) {
				QGraphicsItem * item = (*it);
				if ( item->data( TagName ) == tag ) {
					T *group = qgraphicsitem_cast<T *>((*it));
					Q_ASSERT_X( group, __FUNCTION__, "unable to item name" );
					if ( group ) {
						return group;
					}
				}
		}
		return NULL;
	}
};

#endif
