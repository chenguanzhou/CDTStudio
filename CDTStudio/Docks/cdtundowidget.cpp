/***************************************************************************
    qgsundowidget.cpp
    ---------------------
    begin                : June 2009
    copyright            : (C) 2009 by Martin Dobias
    email                : wonder dot sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "cdtundowidget.h"

#include "qgsmaplayer.h"
#include "qgsmapcanvas.h"
#include "qgsapplication.h"
#include "cdtsegmentationlayer.h"
#include "cdtimagelayer.h"
#include "cdtextractionlayer.h"
#include "cdtclassification.h"

CDTUndoWidget::CDTUndoWidget( QWidget * parent, QgsMapCanvas * mapCanvas )
    : CDTDockWidget( parent )
{
    setupUi( this );
    setWidget( dockWidgetContents );

    connect( undoButton, SIGNAL( clicked() ), this, SLOT( undo( ) ) );
    connect( redoButton, SIGNAL( clicked() ), this, SLOT( redo( ) ) );

    undoButton->setDisabled( true );
    redoButton->setDisabled( true );
    mMapCanvas = mapCanvas;
    mUndoView = new QUndoView( dockWidgetContents );
    gridLayout->addWidget( mUndoView, 0, 0, 1, 2 );
    mUndoStack = NULL;
    mPreviousIndex = 0;
    mPreviousCount = 0;
}

void CDTUndoWidget::setUndoStack( QUndoStack* undoStack )
{
    if ( mUndoView != NULL )
    {
        mUndoView->close();
        delete mUndoView;
        mUndoView = NULL;
    }

    mUndoStack = undoStack;
    mPreviousIndex = mUndoStack->index();
    mPreviousCount = mUndoStack->count();

    mUndoView = new QUndoView( dockWidgetContents );
    mUndoView->setStack( undoStack );
    mUndoView->setObjectName( "undoView" );
    gridLayout->addWidget( mUndoView, 0, 0, 1, 2 );
    setWidget( dockWidgetContents );
    connect( mUndoStack, SIGNAL( canUndoChanged( bool ) ), this, SLOT( undoChanged( bool ) ) );
    connect( mUndoStack, SIGNAL( canRedoChanged( bool ) ), this, SLOT( redoChanged( bool ) ) );
    connect( mUndoStack, SIGNAL( indexChanged( int ) ), this, SLOT( indexChanged( int ) ) );

    undoButton->setDisabled( !mUndoStack->canUndo() );
    redoButton->setDisabled( !mUndoStack->canRedo() );
}

void CDTUndoWidget::destroyStack()
{
    if ( mUndoStack != NULL )
    {
        mUndoStack = NULL;
    }
    if ( mUndoView != NULL )
    {
        mUndoView->close();
        delete mUndoView;
        mUndoView = new QUndoView( dockWidgetContents );
        gridLayout->addWidget( mUndoView, 0, 0, 1, 2 );
    }
}

void CDTUndoWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (qobject_cast<CDTSegmentationLayer*>(layer) ||
        qobject_cast<CDTExtractionLayer*>(layer) ||
        qobject_cast<CDTClassification*>(layer) )
    setMapCanvas(layer->canvas());
    setMapLayer(layer->canvasLayer());
}

void CDTUndoWidget::onCurrentProjectClosed()
{
    destroyStack();
}

void CDTUndoWidget::setMapLayer( QgsMapLayer * layer )
{
    if ( layer != NULL )
    {
        setUndoStack( layer->undoStack() );
    }
    else
    {
        destroyStack();
    }
    emit undoStackChanged();
}

void CDTUndoWidget::setMapCanvas(QgsMapCanvas *mapCanvas)
{
    if (mMapCanvas == mapCanvas)
    {
        return;
    }
    destroyStack();
    mMapCanvas = mapCanvas;
}

void CDTUndoWidget::undoChanged( bool value )
{
    undoButton->setDisabled( !value );
    emit undoStackChanged();
}

void CDTUndoWidget::redoChanged( bool value )
{
    redoButton->setDisabled( !value );
    emit undoStackChanged();
}

void CDTUndoWidget::indexChanged( int curIndx )
{
    int curCount = 0;
    bool canRedo = true;
    if ( mUndoStack )
    {
        canRedo = mUndoStack->canRedo();
        curCount = mUndoStack->count();
    }
    int offset = qAbs( mPreviousIndex - curIndx );

    bool lastRedo = ( mPreviousIndex == ( mPreviousCount - 1 ) && mPreviousCount == curCount && !canRedo );

    if ( offset > 1 || ( offset == 1 && ( canRedo || lastRedo ) ) )
    {
        if ( mMapCanvas )
        {
            mMapCanvas->refresh();
        }
    }

    mPreviousIndex = curIndx;
    mPreviousCount = curCount;
}

void CDTUndoWidget::undo( )
{
    if ( mUndoStack )
        mUndoStack->undo();
}

void CDTUndoWidget::redo()
{
    if ( mUndoStack )
        mUndoStack->redo();
}


void CDTUndoWidget::setupUi( QDockWidget *UndoWidget )
{
    if ( UndoWidget->objectName().isEmpty() )
        UndoWidget->setObjectName( QString::fromUtf8( "UndoWidget" ) );
    UndoWidget->resize( 200, 223 );
    UndoWidget->setMinimumSize( QSize( 200, 220 ) );
    dockWidgetContents = new QWidget( UndoWidget );
    dockWidgetContents->setObjectName( QString::fromUtf8( "dockWidgetContents" ) );
    gridLayout = new QGridLayout( dockWidgetContents );
    gridLayout->setObjectName( QString::fromUtf8( "gridLayout" ) );
    gridLayout->setContentsMargins( 0, 0, 0, 0 );
    spacerItem = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

    gridLayout->addItem( spacerItem, 0, 0, 1, 1 );

    undoButton = new QPushButton( dockWidgetContents );
    undoButton->setObjectName( QString::fromUtf8( "undoButton" ) );
    undoButton->setIcon( QIcon(":/Icon/Undo.png") );

    gridLayout->addWidget( undoButton, 1, 0, 1, 1 );

    redoButton = new QPushButton( dockWidgetContents );
    redoButton->setObjectName( QString::fromUtf8( "redoButton" ) );
    redoButton->setIcon( QIcon(":/Icon/Redo.png") );

    gridLayout->addWidget( redoButton, 1, 1, 1, 1 );

    spacerItem1 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

    gridLayout->addItem( spacerItem1, 0, 1, 1, 1 );

    UndoWidget->setWidget( dockWidgetContents );

    retranslateUi( UndoWidget );

    QMetaObject::connectSlotsByName( UndoWidget );
} // setupUi

void CDTUndoWidget::retranslateUi( QDockWidget *UndoWidget )
{
    UndoWidget->setWindowTitle( QApplication::translate( "UndoWidget", "Undo/Redo", 0, QApplication::UnicodeUTF8 ) );
    undoButton->setText( QApplication::translate( "UndoWidget", "Undo", 0, QApplication::UnicodeUTF8 ) );
    redoButton->setText( QApplication::translate( "UndoWidget", "Redo", 0, QApplication::UnicodeUTF8 ) );
    Q_UNUSED( UndoWidget );
}

