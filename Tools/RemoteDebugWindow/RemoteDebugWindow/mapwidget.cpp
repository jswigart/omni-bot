#include <QtGui>
#include <QtOpenGL/QtOpenGL>

#include "mapwidget.h"

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
enum UserDataKeys { PathKey = 1 };

QGraphicsItemGroup * MapWidget::findGroupForPath( const QString & path ) {	

	QGraphicsItemGroup *lookInGroup = rootGroup;

	const int lastSlash = path.lastIndexOf( "/" );
	if ( lastSlash != -1 ) {
		QString groupPath = path.leftRef( lastSlash ).toString();
		return findGroupForPath( groupPath );
	}

	// look for a child group of this name
	QList<QGraphicsItem *> & children = lookInGroup->children();
	for ( QList<QGraphicsItem *>::iterator it = children.begin();
		it != children.end(); 
		++it ) {
		QGraphicsItem * item = (*it);
		if ( item->data( PathKey ) == path ) {
			QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>((*it));
			Q_ASSERT_X( group, __FUNCTION__, "unable to item name" );
			if ( group ) {
				return group;
			}
		}
	}

	QGraphicsItemGroup * newGroup = new QGraphicsItemGroup;
	newGroup->setData( PathKey, path );
	newGroup->setParentItem( lookInGroup );
	return newGroup;
}

QGraphicsItem * MapWidget::findItemInGroup( const QString & path, QString & name, QGraphicsItemGroup * group ) {
	name = path.midRef( path.lastIndexOf( "/" ) + 1 ).toString();

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

bool MapWidget::msgCircle( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	float x = 0.0f, y = 0.0f, rad = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;
	QString path;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readFloat16( x, 0 );
	db.readFloat16( y, 0 );
	db.readFloat16( rad, 0 );
	db.readInt8( r );
	db.readInt8( g );
	db.readInt8( b );
	db.readInt8( a );

	QGraphicsItemGroup * itemGroup = findGroupForPath( path );

	QString name;
	QGraphicsItem * existingItem = findItemInGroup( path, name, itemGroup );

	QGraphicsEllipseItem * item = qgraphicsitem_cast<QGraphicsEllipseItem*>( existingItem );
	if ( !item ) {
		item = scene()->addEllipse( x - rad * 0.5, y - rad * 0.5, rad, rad );
		item->setData( PathKey, name );
	} else {
		item->setRect( x - rad * 0.5, y - rad * 0.5, rad, rad );
	}
	QPen pen( qRgba( r, g, b, a ) );
	item->setPen( pen );
	item->setParentItem( itemGroup );
	return false;
}

bool MapWidget::msgLine( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;
	QString path;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readFloat16( x0, 0 );
	db.readFloat16( y0, 0 );
	db.readFloat16( x1, 0 );
	db.readFloat16( y1, 0 );
	db.readInt8( r );
	db.readInt8( g );
	db.readInt8( b );
	db.readInt8( a );

	QGraphicsItemGroup * itemGroup = findGroupForPath( path );

	QString name;
	QGraphicsItem * existingItem = findItemInGroup( path, name, itemGroup );

	QGraphicsLineItem * item = qgraphicsitem_cast<QGraphicsLineItem *>( existingItem );
	if ( !item ) {
		item = scene()->addLine( x0, y0, x1, y1 );
		item->setData( PathKey, name );
	} else {
		item->setLine( x0, y0, x1, y1 );
	}
	
	const QRgb color = qRgba( r, g, b, a );
	if ( item->pen().color() != color ) {
		item->setPen( QPen( color ) );
	}

	item->setParentItem( itemGroup );
	return false;
}

bool MapWidget::msgObb( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, yaw = 0.0f;
	int8 r = 0, g = 0, b = 0, a = 0;
	QString path;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readFloat16( x, 0 );
	db.readFloat16( y, 0 );
	db.readFloat16( w, 0 );
	db.readFloat16( h, 0 );
	db.readFloat16( yaw, 0 );
	db.readInt8( r );
	db.readInt8( g );
	db.readInt8( b );
	db.readInt8( a );

	QGraphicsItemGroup * itemGroup = findGroupForPath( path );

	QString name;
	QGraphicsItem * existingItem = findItemInGroup( path, name, itemGroup );

	QGraphicsRectItem * item = qgraphicsitem_cast<QGraphicsRectItem *>( existingItem );
	if ( !item ) {
		item = scene()->addRect( x, y, w, h );
		item->setData( PathKey, name );
	} else {
		item->setRect( x, y, w, h );
	}

	const QRgb color = qRgba( r, g, b, a );
	if ( item->pen().color() != color ) {
		item->setPen( QPen( color ) );
	}
	
	item->setParentItem( itemGroup );
	return false;
}

bool MapWidget::msgImage( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	int8 r = 0, g = 0, b = 0;
	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, yaw = 0.0f;
	int32 clr = 0;
	QString path, imageFile, imageOverlay;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readString( buffer, BufferSz ); imageFile = buffer;
	db.readString( buffer, BufferSz ); imageOverlay = buffer;
	db.readFloat16( x, 0 );
	db.readFloat16( y, 0 );
	db.readFloat16( w, 0 );
	db.readFloat16( h, 0 );
	db.readFloat16( yaw, 0 );
	db.readInt8( r );
	db.readInt8( g );
	db.readInt8( b );

	QGraphicsItemGroup * itemGroup = findGroupForPath( path );

	QString name;
	QGraphicsItem * existingItem = findItemInGroup( path, name, itemGroup );

	QGraphicsPixmapItem * item = qgraphicsitem_cast<QGraphicsPixmapItem *>( existingItem );
	if ( !item ) {
		QPixmap pix( w, h );
		if ( !pix.load( QString( "images/" ) + imageFile ) ) {
			pix.fill( Qt::magenta );
		}
		item = scene()->addPixmap( pix );
		item->setOffset( -w*0.5f, -h*0.5f );
		item->setData( PathKey, name );
		item->setToolTip( name );
		item->setCursor( Qt::CrossCursor );
		item->setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );
	
		/*QGraphicsColorizeEffect * colorize = ;
		colorize->setColor( qRgb( r,g,b ) );*/
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
