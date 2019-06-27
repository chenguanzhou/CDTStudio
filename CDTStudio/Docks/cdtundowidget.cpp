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

#include "stable.h"
//#include "qgsmaplayer.h"
//#include "qgsmapcanvas.h"
//#include "qgsapplication.h"
#include "cdtsegmentationlayer.h"
#include "cdtimagelayer.h"
#include "cdtextractionlayer.h"
#include "cdtclassificationlayer.h"
#include "mainwindow.h"

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
    gridLayout->addWidget( mUndoView, 1, 0, 1, 2 );
    mUndoStack = Q_NULLPTR;
    mPreviousIndex = 0;
    mPreviousCount = 0;
    qDebug("Constructed");
}

void CDTUndoWidget::setUndoStack( QUndoStack* undoStack )
{
    if ( mUndoView != Q_NULLPTR )
    {
        mUndoView->close();
        delete mUndoView;
        mUndoView = Q_NULLPTR;
    }

    mUndoStack = undoStack;
    mPreviousIndex = mUndoStack->index();
    mPreviousCount = mUndoStack->count();

    mUndoView = new QUndoView( dockWidgetContents );
    mUndoView->setStack( undoStack );
    mUndoView->setObjectName( "undoView" );
    gridLayout->addWidget( mUndoView, 1, 0, 1, 2 );
    setWidget( dockWidgetContents );
    connect( mUndoStack, SIGNAL( canUndoChanged( bool ) ), this, SLOT( undoChanged( bool ) ) );
    connect( mUndoStack, SIGNAL( canRedoChanged( bool ) ), this, SLOT( redoChanged( bool ) ) );
    connect( mUndoStack, SIGNAL( indexChanged( int ) ), this, SLOT( indexChanged( int ) ) );

    undoButton->setDisabled( !mUndoStack->canUndo() );
    redoButton->setDisabled( !mUndoStack->canRedo() );
}

void CDTUndoWidget::destroyStack()
{
    if ( mUndoStack != Q_NULLPTR )
    {
        mUndoStack = Q_NULLPTR;
    }
    if ( mUndoView != Q_NULLPTR )
    {
        mUndoView->close();
        delete mUndoView;
        mUndoView = new QUndoView( dockWidgetContents );
        gridLayout->addWidget( mUndoView, 1, 0, 1, 2 );
    }
}

void CDTUndoWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    Q_UNUSED(layer);
    onDockClear();
}

void CDTUndoWidget::onDockClear()
{
    destroyStack();
    this->setEnabled(false);
    this->setVisible(false);
}

void CDTUndoWidget::setLayer(CDTBaseLayer *layer)
{
    onDockClear();
    if (qobject_cast<CDTSegmentationLayer*>(layer) ||
        qobject_cast<CDTExtractionLayer*>(layer) ||
        qobject_cast<CDTClassificationLayer*>(layer) )
    {
        setMapCanvas(layer->canvas());
        setMapLayer(layer->canvasLayer());
        this->setEnabled(true);
        this->setVisible(true);
        this->raise();
        this->adjustSize();
    }
}

void CDTUndoWidget::setMapLayer( QgsMapLayer * layer )
{
    if ( layer != Q_NULLPTR )
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
    dockWidgetContents = new QWidget( UndoWidget );
    dockWidgetContents->setObjectName( QString::fromUtf8( "dockWidgetContents" ) );
    gridLayout = new QGridLayout( dockWidgetContents );
    gridLayout->setObjectName( QString::fromUtf8( "gridLayout" ) );
    gridLayout->setContentsMargins( 0, 0, 0, 0 );
//    spacerItem = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

//    gridLayout->addItem( spacerItem, 0, 0, 1, 1 );

    undoButton = new QPushButton( dockWidgetContents );
    undoButton->setObjectName( QString::fromUtf8( "undoButton" ) );
    undoButton->setIcon( QIcon(":/Icons/Undo.png") );
    undoButton->setFlat(true);
    undoButton->setIconSize(MainWindow::getIconSize());

    gridLayout->addWidget( undoButton, 0, 0, 1, 1 );

    redoButton = new QPushButton( dockWidgetContents );
    redoButton->setObjectName( QString::fromUtf8( "redoButton" ) );
    redoButton->setIcon( QIcon(":/Icons/Redo.png") );
    redoButton->setFlat(true);
    redoButton->setIconSize(MainWindow::getIconSize());

    gridLayout->addWidget( redoButton, 0, 1, 1, 1 );

//    spacerItem1 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

//    gridLayout->addItem( spacerItem1, 0, 1, 1, 1 );

    UndoWidget->setWidget( dockWidgetContents );

    retranslateUi( UndoWidget );

    QMetaObject::connectSlotsByName( UndoWidget );
} // setupUi

void CDTUndoWidget::retranslateUi( QDockWidget *UndoWidget )
{
    UndoWidget->setWindowTitle( QApplication::translate( "UndoWidget", "Undo/Redo", 0 ));
    undoButton->setText( QApplication::translate( "UndoWidget", "Undo", 0) );
    redoButton->setText( QApplication::translate( "UndoWidget", "Redo", 0 ) );
    Q_UNUSED( UndoWidget );
}

