/***************************************************************************
    qgsundowidget.h
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
#ifndef QGSUNDOWIDGET_H
#define QGSUNDOWIDGET_H


//#include <QAction>
//#include <QApplication>
//#include <QButtonGroup>
//#include <QDockWidget>
//#include <QGridLayout>
//#include <QPushButton>
//#include <QSpacerItem>
//#include <QWidget>
//#include <QUndoView>
//#include <QUndoStack>
#include "cdtdockwidget.h"
#include "log4qt/logger.h"

class QUndoStack;
class QUndoView;
class QGridLayout;
class QPushButton;
class QgsMapCanvas;
class QgsMapLayer;


/**
 * Class that handles undo display fo undo commands
 */
class CDTUndoWidget : public CDTDockWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
  public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
//    QSpacerItem *spacerItem;
    QPushButton *undoButton;
    QPushButton *redoButton;
//    QSpacerItem *spacerItem1;

    CDTUndoWidget( QWidget * parent, QgsMapCanvas* mapCanvas );
    void setupUi( QDockWidget *UndoWidget );
    void retranslateUi( QDockWidget *UndoWidget );

    void setUndoStack( QUndoStack * undoStack );
    void destroyStack();

    QWidget* dockContents() { return dockWidgetContents; }

  public slots:
    void setCurrentLayer(CDTBaseLayer* layer);
    void onDockClear();

    void setLayer(CDTBaseLayer* layer);

    void setMapCanvas(QgsMapCanvas* mapCanvas);
    void setMapLayer( QgsMapLayer * layer );
    void undoChanged( bool value );
    void redoChanged( bool value );
    void indexChanged( int curIndx );
    void undo();
    void redo();

  signals:
    void undoStackChanged();

  private:
    QUndoView * mUndoView;
    QUndoStack * mUndoStack;
    QgsMapCanvas* mMapCanvas;

    int mPreviousIndex;
    int mPreviousCount;
};


#endif // QGSUNDOWIDGET_H

