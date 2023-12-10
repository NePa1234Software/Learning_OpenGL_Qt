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

#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Qt QOpenGLWidget Lesson 1b");
    QCoreApplication::setOrganizationName("Bla");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    //! [1]
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);

    // Enable stereoscopic rendering support
    format.setStereo(true);

    // Request OpenGL 3.3 core or OpenGL ES 3.0.
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        qDebug("Requesting 3.3 core context");
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    } else {
        qDebug("Requesting 3.0 context");
        format.setVersion(3, 0);
    }

    QSurfaceFormat::setDefaultFormat(format);
    //! [1]

    MainWindow mw;
    mw.resize(800, 400);
    mw.show();

    return a.exec();
}
