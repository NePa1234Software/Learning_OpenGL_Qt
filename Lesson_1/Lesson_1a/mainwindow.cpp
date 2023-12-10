//-----------------------------------------------------------------------------
// Author: Neil Parker
// Date: 12/2023
//
// Acklowledgement: I am only learning OpenGL and its usage with Qt
// 1) Code is based on the Udemy course from
//    Steve Jones at the Game Institute
// 2) The project start is based on one the many Qt OpenGL example
//
// SPDX-License-Identifier: GPL-3.0-or-later
//-----------------------------------------------------------------------------

#include "mainwindow.h"
#include "glwidget.h"

#include <QMainWindow>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    resize(500,500);

    GLWidget *glwidget = new GLWidget(this);
    setCentralWidget(glwidget);
}
