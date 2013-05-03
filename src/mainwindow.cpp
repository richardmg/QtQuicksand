/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "mainwindow.h"
#include "widget.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent,flags)
{
    w = new Widget(this);
    setCentralWidget(w);
}

MainWindow::~MainWindow()
{
}
