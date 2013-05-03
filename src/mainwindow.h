/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Widget;
class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

private:
    Widget* w;
};

#endif // MAINWINDOW_H
