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

	rootGroup = new QGraphicsItemGroup();
	scene()->addItem( rootGroup );
}

void MapWidget::resizeEvent( QResizeEvent * event ) {
	const QRect newSize = rect();
	QRectF sceneRect = scene()->sceneRect();

	setSceneRect( sceneRect );
	setResizeAnchor( QGraphicsView::AnchorViewCenter );
	
	horizontalScrollBar()->setMinimum( sceneRect.x() );
	horizontalScrollBar()->setMaximum( sceneRect.width() );
	horizontalScrollBar()->setPageStep( newSize.width() );
	horizontalScrollBar()->setSingleStep( 1 );

	verticalScrollBar()->setMinimum( sceneRect.y() );
	verticalScrollBar()->setMaximum( sceneRect.height() );	
	verticalScrollBar()->setPageStep( newSize.height() );
	verticalScrollBar()->setSingleStep( 1 );
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

bool MapWidget::msgCirleInternal( RemoteLib::DataBuffer & db, QGraphicsItemGroup * subGroup ) {
	enum { BufferSz = 256 };
	char groupBuf[ BufferSz ] = {};
	char nameBuf[ BufferSz ] = {};

	float x = 0.0f, y = 0.0f, rad = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;
	
	const bool unpacked = RemoteLib::UnpackCircle( db,
		groupBuf, BufferSz,
		nameBuf, BufferSz, x, y, rad, 
		r, g, b, a );

	QGraphicsItemGroup * itemGroup = findGroup( groupBuf, subGroup );

	QGraphicsItem * existingItem = findItemInGroup( nameBuf, itemGroup );

	QGraphicsEllipseItem * item = qgraphicsitem_cast<QGraphicsEllipseItem*>( existingItem );
	if ( !item ) {
		item = scene()->addEllipse( x - rad * 0.5, y - rad * 0.5, rad, rad );
		item->setData( PathKey, nameBuf );
	} else {
		item->setRect( x - rad * 0.5, y - rad * 0.5, rad, rad );
	}
	QPen pen( qRgba( r, g, b, a ) );
	item->setPen( pen );
	item->setParentItem( itemGroup );
	return !db.hasReadError();
}

bool MapWidget::msgLineInternal( RemoteLib::DataBuffer & db, QGraphicsItemGroup * subGroup ) {
	enum { BufferSz = 256 };
	char groupBuf[ BufferSz ] = {};
	char nameBuf[ BufferSz ] = {};

	float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;
	const bool unpacked = RemoteLib::UnPackLine( db,
		groupBuf, BufferSz,
		nameBuf, BufferSz,
		x0, y0, x1, y1,
		r, g, b, a );

	QGraphicsItemGroup * itemGroup = findGroup( groupBuf, subGroup );
	QGraphicsItem * existingItem = findItemInGroup( nameBuf, itemGroup );

	QGraphicsLineItem * item = qgraphicsitem_cast<QGraphicsLineItem *>( existingItem );
	if ( !item ) {
		item = scene()->addLine( x0, y0, x1, y1 );
		item->setData( PathKey, nameBuf );
	} else {
		item->setLine( x0, y0, x1, y1 );
	}

	const QRgb color = qRgba( r, g, b, a );
	if ( item->pen().color() != color ) {
		item->setPen( QPen( color ) );
	}

	item->setParentItem( itemGroup );
	return !db.hasReadError();
}

bool MapWidget::msgRectInternal( RemoteLib::DataBuffer & db, QGraphicsItemGroup * subGroup ) {
	enum { BufferSz = 256 };
	char groupBuf[ BufferSz ] = {};
	char nameBuf[ BufferSz ] = {};

	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, yaw = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;

	const bool unpacked = RemoteLib::UnPackRect( db, 
		groupBuf, BufferSz,
		nameBuf, BufferSz,
		x, y, w, h, yaw,
		r, g, b, a );

	QGraphicsItemGroup * itemGroup = findGroup( groupBuf, subGroup );
	QGraphicsItem * existingItem = findItemInGroup( nameBuf, itemGroup );

	QGraphicsRectItem * item = qgraphicsitem_cast<QGraphicsRectItem *>( existingItem );
	if ( !item ) {
		item = scene()->addRect( x-w*0.5f, y-h*0.5f, w, h );
		item->setData( PathKey, nameBuf );
		item->setToolTip( nameBuf );
	} else {
		item->resetTransform();
		item->setRect( -w*0.5f, -h*0.5f, w*0.5f, h*0.5f );
		item->rotate( 90-yaw );
	}

	item->setPos( x, y );

	const QRgb color = qRgba( r, g, b, a );
	if ( item->pen().color() != color ) {
		item->setPen( QPen( color ) );
	}

	item->setParentItem( itemGroup );
	return !db.hasReadError();
}

bool MapWidget::msgImageInternal( RemoteLib::DataBuffer & db, QGraphicsItemGroup * subGroup ) {
	enum { BufferSz = 256 };
	char groupBuf[ BufferSz ] = {};
	char nameBuf[ BufferSz ] = {};
	char fileBuf[ BufferSz ] = {};

	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, yaw = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;

	const bool unpacked = RemoteLib::UnpackImage( db, 
		groupBuf, BufferSz, 
		nameBuf, BufferSz, 
		fileBuf, BufferSz,
		x, y, w, h, yaw,
		r, g, b, a );

	QGraphicsItemGroup * itemGroup = findGroup( groupBuf, subGroup );
	QGraphicsItem * existingItem = findItemInGroup( nameBuf, itemGroup );

	QGraphicsPixmapItem * item = qgraphicsitem_cast<QGraphicsPixmapItem *>( existingItem );
	if ( !item ) {
		QPixmap pix( w, h );
		if ( !pix.load( QString( "images/" ) + fileBuf ) ) {
			pix.fill( Qt::magenta );
		}
		item = scene()->addPixmap( pix );
		item->setOffset( -pix.width()*0.5f, -pix.height()*0.5f );
		item->setData( PathKey, nameBuf );
		item->setToolTip( nameBuf );
		item->setCursor( Qt::CrossCursor );
		item->setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );
		item->setGraphicsEffect( new QGraphicsColorizeEffect() );
	} else {		
		// todo:
	}

	QGraphicsColorizeEffect *colorize = static_cast<QGraphicsColorizeEffect*>( item->graphicsEffect() );
	if ( colorize ) {
		colorize->setColor( qRgb( r,g,b ) );
	}

	item->setPos( x, y );
	item->setParentItem( itemGroup );
	return !db.hasReadError();
}

bool MapWidget::msgCircle( RemoteLib::DataBuffer & db ) {
	return msgCirleInternal( db, rootGroup );
}

bool MapWidget::msgLine( RemoteLib::DataBuffer & db ) {
	return msgLineInternal( db, rootGroup );
}

bool MapWidget::msgRect( RemoteLib::DataBuffer & db ) {
	return msgRectInternal( db, rootGroup );
}

bool MapWidget::msgImage( RemoteLib::DataBuffer & db ) {
	return msgImageInternal( db, rootGroup );
}

bool MapWidget::msgToken( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 256 };
	char groupBuf[ BufferSz ] = {};
	char nameBuf[ BufferSz ] = {};
	db.readSmallString( groupBuf, BufferSz );
	db.readSmallString( nameBuf, BufferSz );

	// find the group object
	QGraphicsItemGroup * itemGrp = findGroup( groupBuf );
	QGraphicsItemGroup * tokenGrp = findGroup( nameBuf, itemGrp );

	int32 subTag = 0;
	while( db.readInt32( subTag ) && subTag != RemoteLib::ID_tokenEnd ) {
		switch( subTag ) 
		{
		case RemoteLib::ID_circle:
			{
				msgCirleInternal( db, tokenGrp );
				break;
			}
		case RemoteLib::ID_ellipse:
			{
				//enum { BufferSize = 64 };
				//char tagBuffer[BufferSize] = {};
				//int8 r = 0, g = 0, b = 0, a = 0;
				//float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, yaw = 0.0f;
				//float pieAngle = 0.0f;
				//const bool unpacked = UnpackEllipse( db, 
				//	tagBuffer, BufferSize,
				//	x, y, w, h, pieAngle, yaw, r, g, b, a );

				//Q_ASSERT_X( unpacked, __FUNCTION__, "unable to unpack ellipse" );

				//QGraphicsEllipseItem * item = findItem<QGraphicsEllipseItem>( tagBuffer, tokenGrp );
				//if ( !item ) {
				//	item = new QGraphicsEllipseItem;	
				//	item->setData( TagName, tagBuffer );
				//	item->setParentItem( tokenGrp );
				//} else {		
				//	// todo:
				//}

				//const QRgb color = qRgba( r, g, b, a );
				//if ( item->pen().color() != color ) {
				//	item->setPen( QPen( color ) );
				//}

				//const int range = 360 * 16;
				//float startAngle = yaw - pieAngle * 0.5f;
				//if ( startAngle < 0.0f ) {
				//	startAngle += range;
				//} else if ( startAngle > range ) {
				//	startAngle -= range;
				//}

				//item->setStartAngle( (int)range );
				//item->setSpanAngle( (int)(pieAngle * 16.0f) % range );
				//item->setRect( x, y, w, h );
				////item->setPos( x, y );
				break;
			}
		case RemoteLib::ID_line:
			{
				msgLineInternal( db, tokenGrp );
				break;
			}
		case RemoteLib::ID_obb:
			{
				msgRectInternal( db, tokenGrp );
				break;
			}
		case RemoteLib::ID_image:
			{
				msgImageInternal( db, tokenGrp );
				break;
			}
		default:
			Q_ASSERT_X( 0, __FUNCTION__, "unknown token subtype" );
			break;
		}
	}
	return !db.hasReadError();
}
