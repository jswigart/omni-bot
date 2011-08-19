#include <QtGui>
#include <QtOpenGL/QtOpenGL>

#include "mapwidget.h"

#include "messageTags.h"
#include "dataBuffer.h"

//////////////////////////////////////////////////////////////////////////

MapScene::MapScene() {
	setItemIndexMethod( QGraphicsScene::BspTreeIndex );
}

void MapScene::contextMenuEvent( QGraphicsSceneContextMenuEvent * contextMenuEvent ) {
}

void MapScene::keyPressEvent( QKeyEvent * keyEvent ) {
	if ( keyEvent->key() == Qt::Key_Tab ) {

	}
}

void MapScene::keyReleaseEvent( QKeyEvent * keyEvent ) {
}

void MapScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * mouseEvent ) {
}

void MapScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent ) {
}

void MapScene::mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent ) {
}

void MapScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent ) {
}

//////////////////////////////////////////////////////////////////////////

MapWidget::MapWidget( QWidget *parent )
: QGraphicsView( parent ) {
	//setViewport( new QGLWidget ); 
	setRenderHints( QPainter::HighQualityAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
	
	setScene( &world );
	setResizeAnchor( QGraphicsView::AnchorViewCenter );

	rootGroup = new QGraphicsItemGroup();
	scene()->addItem( rootGroup );
}

void MapWidget::resizeEvent( QResizeEvent * event ) {
}

void MapWidget::dragEnterEvent( QDragEnterEvent *event ) {
}

void MapWidget::dragLeaveEvent( QDragLeaveEvent * event ) {
}

void MapWidget::dropEvent( QDropEvent *event ) {
}

void MapWidget::dragMoveEvent ( QDragMoveEvent * event ) {
}

void MapWidget::wheelEvent( QWheelEvent * event ) {
	float scrollAmount = 0.05f;
	float scaleX = 1.0f + ((event->delta() > 0) ? 1.0f : -1.0f) * scrollAmount;
	float scaleY = 1.0f + ((event->delta() > 0) ? 1.0f : -1.0f) * scrollAmount;
	scale( scaleX, scaleY );
	event->accept();
}

void MapWidget::splitPath( const QString & fullPath, QString & subPath, QString & name ) {
	const int lastSlash = fullPath.lastIndexOf( "/" );
	if ( lastSlash != -1 ) {
		subPath = fullPath.leftRef( lastSlash ).toString();
		name = fullPath.midRef( lastSlash ).toString();
		return;
	}
	Q_ASSERT_X( 0, __FUNCTION__, "invalid path" );
}

QGraphicsItemGroup * MapWidget::findGroup( const QString & path, QGraphicsItemGroup * group ) {	
	// look for a child group of this name	
	QList<QGraphicsItem *> & children = group ? group->children() : rootGroup->children();
	for ( QList<QGraphicsItem *>::iterator it = children.begin();
		it != children.end(); 
		++it ) {
			QGraphicsItem * item = (*it);
			if ( item->data( PathKey ) == path ) {
				QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>((*it));
				Q_ASSERT_X( group, __FUNCTION__, "unable to item name" );
				if ( group ) {
					QPointF p = group->pos();
					return group;
				}
			}
	}

	QGraphicsItemGroup * newGroup = new QGraphicsItemGroup;
	newGroup->setData( PathKey, path );
	if ( group ) {
		newGroup->setParentItem( group );
	} else {
		newGroup->setParentItem( rootGroup );
	}
	return newGroup;
}

//QGraphicsItemGroup * MapWidget::findGroupForPath( const QString & path ) {	
//
//	QGraphicsItemGroup *lookInGroup = rootGroup;
//
//	const int lastSlash = path.lastIndexOf( "/" );
//	if ( lastSlash != -1 ) {
//		QString groupPath = path.leftRef( lastSlash ).toString();
//		return findGroupForPath( groupPath );
//	}
//
//	// look for a child group of this name
//	QList<QGraphicsItem *> & children = lookInGroup->children();
//	for ( QList<QGraphicsItem *>::iterator it = children.begin();
//		it != children.end(); 
//		++it ) {
//		QGraphicsItem * item = (*it);
//		if ( item->data( PathKey ) == path ) {
//			QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>((*it));
//			Q_ASSERT_X( group, __FUNCTION__, "unable to item name" );
//			if ( group ) {
//				return group;
//			}
//		}
//	}
//
//	QGraphicsItemGroup * newGroup = new QGraphicsItemGroup;
//	newGroup->setData( PathKey, path );
//	newGroup->setParentItem( lookInGroup );
//	return newGroup;
//}

QGraphicsItem * MapWidget::findItemInGroup( const QString & name, QGraphicsItemGroup * group ) {
	QGraphicsItem * existingItem = NULL;

	// look for the item in this group
	QList<QGraphicsItem *> & children = group->children();
	QList<QGraphicsItem *>::iterator it;
	for ( it = children.begin(); it != children.end(); ++it ) {
		if ( (*it)->data( PathKey ).toString() == name ) {
			existingItem = (*it);
			break;
		}
	}
	return existingItem;
}

bool MapWidget::msgDelete( const QString & group, const QString & name ) {
	QGraphicsItemGroup * itemGroup = findGroup( group, rootGroup );
	QGraphicsItem * item = findItemInGroup( name, itemGroup );
	if ( item ) {
		scene()->removeItem( item );
		delete itemGroup;
	}
	return true;
}
