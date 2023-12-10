#pragma once
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

#include "shaderprogram.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <QOpenGLFunctions_3_3_Core>
#include <QTime>

///
/// \brief The GLWidget class uses QOpenGLWidget which will provide the OpenGL context and render target.
/// QOpenGLFunctions_3_3_Core will give access to all OpenGL function of this version.
///
class GLWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core // QOpenGLFunctions for newest
{
public:
    GLWidget(QWidget *parent);

protected:
    // QOpenGLWidget overrides - the context is set by Qt
    void paintGL() override;
    void initializeGL() override;

    // User keyboard interaction
    void keyPressEvent(QKeyEvent *event) override;

    // "update" timer
    void timerEvent(QTimerEvent *event) override;

private slots:

    // QOpenGLWidget signal handlers
    void onFrameSwapped();
    void onAboutToCompose();

private:
    // Helper
    void initializeStatistics();

    // Scene data
    ShaderProgram m_shaderProgram;
    QColor m_background {Qt::red};
    GLuint m_vbo {0};
    GLuint m_ibo {0};
    GLuint m_vao {0};

    // Statistics data
    unsigned int m_frameCount {0};
    qint64 m_nsecsElapsed {0};
    QElapsedTimer m_elapsedTime;
    QTime m_programStart;

    // User interaction
    bool m_wireframeMode {false};
};
