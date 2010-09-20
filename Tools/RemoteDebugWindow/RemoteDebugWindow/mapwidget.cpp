#include <QtGui>
#include <QtOpenGL/QtOpenGL>

#include "mapwidget.h"

#include "dataBuffer.h"

static const int tileSize = 64;

MapWidget::MapWidget( QWidget *parent )
: QGraphicsView( parent ) {
	setViewport( new QGLWidget ); 
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

void MapWidget::mousePressEvent( QMouseEvent *event ) {	
	/*if ( event->button() == Qt::LeftButton ) {
		QList<QGraphicsItem *> selection = items( event->x(), event->y() );
		if ( selection.size() > 0 ) {
			QGraphicsPixmapItem *dragItem = qgraphicsitem_cast<QGraphicsPixmapItem*>( selection[ 0 ] );
			if ( dragItem ) {
				dragItems.clear();
				dragItems.push_back( dragItem );

				QDrag *drag = new QDrag(this);
				QMimeData *mimeData = new QMimeData;

				mimeData->setText( "dragLocal" );
				drag->setMimeData( mimeData );
				drag->setPixmap( dragItem->pixmap() );

				Qt::DropAction dropAction = drag->exec();
			}
		}
	}*/
}

void MapWidget::dragEnterEvent( QDragEnterEvent *event ) {
	/*if ( event->mimeData()->hasUrls() ) {
		event->acceptProposedAction();

		for( int i = 0; i < event->mimeData()->urls().size(); ++i ) {
			QDir dragFilePath = event->mimeData()->urls()[ i ].toLocalFile();
			QPixmap newImage( dragFilePath.path() );
			
			QFileInfo dragFileInfo( dragFilePath.path() );
			QFile dragFile( dragFilePath.path() );

			QString copyToFile = QString( "data/scenarios/" ) + dragFileInfo.fileName();
			dragFile.copy( copyToFile );

			dragItems.push_back( scene()->addPixmap( newImage ) );
			
			QSize imageSize = newImage.size();
			dragItems.back()->setOffset( -imageSize.width() * 0.5f, -imageSize.height() * 0.5f );
			SnapToGrid( event->pos(), dragItems.back() );
		}		
	}
	if ( event->mimeData()->hasText() ) {
		if ( event->mimeData()->text() == "dragLocal" ) {
			event->acceptProposedAction();

			for( int i = 0; i < dragItems.size(); ++i ) {
				SnapToGrid( event->pos(), dragItems[ i ] );
			}
		}
	}*/
}

void MapWidget::dragLeaveEvent( QDragLeaveEvent * event ) {
	/*for( int i = 0; i < dragItems.size(); ++i ) {
		scene()->removeItem( dragItems[ i ] );
		delete dragItems[ i ];
		dragItems[ i ] = NULL;
	}
	dragItems.clear();
	event->accept();*/	
}

void MapWidget::dropEvent( QDropEvent *event ) {
	/*for( int i = 0; i < dragItems.size(); ++i ) {
		SnapToGrid( event->pos(), dragItems[ i ] );
	}
	dragItems.clear();
	event->acceptProposedAction();*/
}

void MapWidget::dragMoveEvent ( QDragMoveEvent * event ) {
	/*for( int i = 0; i < dragItems.size(); ++i ) {
		SnapToGrid( event->pos(), dragItems[ i ] );
	}	
	event->acceptProposedAction();*/
}

void MapWidget::wheelEvent( QWheelEvent * event ) {
	if ( event->delta() > 0 ) {
		scale( 1.1f, 1.1f );
	} else {
		scale( 0.9f, 0.9f );
	}
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

	float x = 0.0f, y = 0.0f, r = 0.0f;
	int32 clr = 0;
	QString path;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readFloat16( x, 0 );
	db.readFloat16( y, 0 );
	db.readFloat16( r, 0 );
	db.readInt32( clr );

	QGraphicsItemGroup * itemGroup = findGroupForPath( path );

	QString name;
	QGraphicsItem * existingItem = findItemInGroup( path, name, itemGroup );

	QGraphicsEllipseItem * item = qgraphicsitem_cast<QGraphicsEllipseItem*>( existingItem );
	if ( !item ) {
		item = scene()->addEllipse( x - r * 0.5, y - r * 0.5, r, r );
		item->setData( PathKey, name );
	} else {
		item->setRect( x - r * 0.5, y - r * 0.5, r, r );
	}
	QPen pen( clr );
	item->setPen( pen );
	item->setParentItem( itemGroup );
	return false;
}

bool MapWidget::msgLine( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
	int32 clr = 0;
	QString path;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readFloat16( x0, 0 );
	db.readFloat16( y0, 0 );
	db.readFloat16( x1, 0 );
	db.readFloat16( y1, 0 );
	db.readInt32( clr );

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
	QPen pen( clr );
	item->setPen( pen );
	item->setParentItem( itemGroup );
	return false;
}

bool MapWidget::msgImage( RemoteLib::DataBuffer & db ) {
	enum { BufferSz = 512 };
	char buffer[ BufferSz ] = {};

	float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;
	int32 clr = 0;
	QString path, imageFile;
	db.readString( buffer, BufferSz ); path = buffer;
	db.readString( buffer, BufferSz ); imageFile = buffer;
	db.readFloat16( x, 0 );
	db.readFloat16( y, 0 );
	db.readFloat16( w, 0 );
	db.readFloat16( h, 0 );

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
	} else {		
		// todo:
	}
	item->setPos( x, y );
	item->setParentItem( itemGroup );
	return !db.hasReadError();
}
