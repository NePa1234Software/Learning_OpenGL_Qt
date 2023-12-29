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
#include "texture2D.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QTime>
#include <QMatrix4x4>
#include <QVector3D>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_fpsCamera(QVector3D(0.0f, 0.0f, 10.0f), QVector3D(0.0f, 0.0f, 0.0f))
    , m_orbitCamera(10.0f, 0.0f, 0.0f)
    , m_orbitalCameraMode(true)
{
    // No need to do any OpenGL stuff here as Qt will
    // Call initializeGL after setting the currect context.

    m_background = Qt::red;
    setMinimumSize(800, 300);
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    qInfo() << "Shutdown : cleanup";
    // Cleanup
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}

void GLWidget::timerEvent(QTimerEvent *event)
{
    // Request the OpenGL context an the call to paintGL
    update();
}

///////////////////////////////////////////////////////////////////////////////
/// OpenGL
///////////////////////////////////////////////////////////////////////////////

void GLWidget::initializeGL()
{
    // Basic initialization

    qInfo() << "Initialize : OpenGL wrapper (Qt)";
    initializeOpenGLFunctions();
    initializeStatistics();

    qInfo() << "Initialize : Vertex Buffer Object (vbo)";
    // Set up an array of vertices for a quad (2 triangls)
    // with an index buffer data
    GLfloat cubeVertices[] = {
        // position		 // tex coords

        // front face
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

        // back face
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

        // left face
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

        // right face
        1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
        1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

        // top face
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

        // bottom face
        -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
    };

    // Cube and floor positions
    m_cubePos = QVector3D(0.0f, 0.0f, 0.0f);
    m_floorPos = QVector3D(0.0f, -1.0f, 0.0f);

    // Set up vertex buffer(s) on the GPU
    // Generate 1 empty vertex buffer on the GPU
    glGenBuffers(1, &m_vbo);

    // "bind", which means set as the current buffer the next command apply to
    // GL_ARRAY_BUFFER (Vertex attributes)
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glBindBuffer.xhtml
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    qInfo() << "Initialize : Vertex Array Object (vao)";
    // Copy the vertex data from CPU to GPU
    // STREAM
    //  The data store contents will be modified once and used at most a few times.
    // STATIC*
    //  The data store contents will be modified once and used many times.
    // DYNAMIC
    //  The data store contents will be modified repeatedly and used many times.
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glBufferData.xhtml
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Request OpenGL to create new Vertex Array Object (VAO)
    glGenVertexArrays(1, &m_vao);

    // "bind" - make it the current one for following commands
    glBindVertexArray(m_vao);

    // Define a layout for the first vertex buffer (index 0)
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glVertexAttribPointer.xhtml
    // 3 floats of data that should not be normalized (data is normalized to the viewport already)
    // Stride of 5 floats (3x4=12 bytes per vertex + 2x4=8 bytes for texture position, stride is then 20)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));

    // Enable the first attribute or attribute index 0
    glEnableVertexAttribArray(0);

    // Same stride but offset is 3*3 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // "unbind" is good to make sure other code doesn't change it elsewhere
    glBindVertexArray(0);

    qInfo() << "Initialize : Shaders ";
    m_shaderProgram.loadShaders(":/Shaders/basictexture3D.vert",
                                ":/Shaders/basictexture3D.frag");

    m_texture.loadTexture(":/Images/funpic.jpg", true);
    m_textureFloor.loadTexture(":/Images/grid.jpg", true);

    qInfo() << "Initialize : DONE ... start the update timer";
    m_programStart = QTime::currentTime();
    startTimer(10);
}

void GLWidget::paintGL()
{
    // NOTE: no logging here, this function is called very often

    // Clear the viewport
    glClearColor(m_background.redF(), m_background.greenF(), m_background.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Just to demonstrate the rendering over time, add some movement
    // Time since app was started
    QTime programRun = QTime::currentTime();
    float timeSecs = float(m_programStart.msecsTo(programRun)) / 1000.0;

    // Set up the MVP matrices
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    // Model TSR (rotate, scale, then translate)
    // no rotate
    // scale
    model.scale(1.0f + sin(timeSecs) * 0.05f);
    // translate
    model.translate(m_cubePos);

    // Create the view matrix using the new camera class
    if (m_orbitalCameraMode)
    {
        view = m_orbitCamera.viewMatrix();
    }
    else
    {
        view = m_fpsCamera.viewMatrix();
    }

    // Create the projection matrix
    //projection.setToIdentity();
    projection.perspective(m_fpsCamera.getFOV(), width()/height(), 0.1f, 100.0f);

    // Render the rectangle (two triangles)
    // Must be called BEFORE setting uniforms because setting uniforms
    // is done on the currently active shader program.
    m_shaderProgram.use();

    // Setup the MVP matrices inside the shaders
    m_shaderProgram.setUniform("model", model);
    m_shaderProgram.setUniform("view", view);
    m_shaderProgram.setUniform("projection", projection);

    m_texture.bind();

    // We want to draw the vertices so "bind" (select) the vao first
    glBindVertexArray(m_vao);

    // The triangles will be drawn with this mode
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframeMode ? GL_LINE : GL_FILL);

    // Draw the "elements" - 0 offset
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Position below the cube and squash it flat
    model.setToIdentity();
    model.translate(m_floorPos);
    model.scale(QVector3D(10.0f, 0.01f, 10.0f));

    // Update the M(VP) matrices inside the shaders
    m_shaderProgram.setUniform("model", model);

    m_textureFloor.bind();

    // Draw the floor using the same squashed cubeVertices
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // "unbind" to ensure no further changes the vao can be made
    glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////
/// UI handling
///////////////////////////////////////////////////////////////////////////////

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    const float speedMove = 0.05f;
    const float speedRotateDeg = 0.5f;
    switch(event->key())
    {
    case Qt::Key_Escape:
        qInfo() << "Application - Escaping ... quit.";
        qApp->quit();
        break;
    case Qt::Key_F1:
        qInfo() << "Application - F1 - fullscreen toggle.";
        if (!window()->isFullScreen())
            window()->showFullScreen();
        else
            window()->showNormal();
        break;
    case Qt::Key_F2:
        m_wireframeMode = !m_wireframeMode;
        qInfo() << "Application - toggle wireframe mode." << m_wireframeMode;
        break;
    case Qt::Key_F3:
        m_orbitalCameraMode = !m_orbitalCameraMode;
        m_fpsCamera.setPosition(QVector3D(0.0f, 0.0f, 10.0f));
        m_fpsCamera.setRotation(0.0f, 0.0f);
        m_orbitCamera.setRadius(10.0f);
        m_orbitCamera.setRotation(0.0f, 0.0f);
        qInfo() << "Application - toggle orbital camera mode." << m_orbitalCameraMode;
        break;
    case Qt::Key_W: // Camera forward (-z)
        if (!m_orbitalCameraMode) m_fpsCamera.move(QVector3D(0.0f, 0.0f, -speedMove));
        if (m_orbitalCameraMode) m_orbitCamera.setRadius(m_orbitCamera.radius()-speedMove);
        break;
    case Qt::Key_S: // Camera backup (+z)
        if (!m_orbitalCameraMode) m_fpsCamera.move(QVector3D(0.0f, 0.0f, speedMove));
        if (m_orbitalCameraMode) m_orbitCamera.setRadius(m_orbitCamera.radius()+speedMove);
        break;
    case Qt::Key_A: // Camera move left
        if (!m_orbitalCameraMode) m_fpsCamera.move(QVector3D(-speedMove, 0.0f, 0.0f));
        break;
    case Qt::Key_D: // Camera move right
        if (!m_orbitalCameraMode) m_fpsCamera.move(QVector3D(speedMove, 0.0f, 0.0f));
        break;
    case Qt::Key_L: // Camera move right
        if (!m_orbitalCameraMode) m_fpsCamera.setLookAt(m_cubePos);
        if (m_orbitalCameraMode) m_orbitCamera.setLookAt(m_cubePos);
        break;
    case Qt::Key_Left: // Yaw to left (right hand rule, rotate around the y/up axis)
        if (!m_orbitalCameraMode) m_fpsCamera.rotate(-speedRotateDeg, 0.0f);
        if (m_orbitalCameraMode) m_orbitCamera.rotate(-speedRotateDeg, 0.0f);
        break;
    case Qt::Key_Right:// Yaw to right (right hand rule, rotate around the y/up axis)
        if (!m_orbitalCameraMode) m_fpsCamera.rotate(speedRotateDeg, 0.0f);
        if (m_orbitalCameraMode) m_orbitCamera.rotate(speedRotateDeg, 0.0f);
        break;
    case Qt::Key_Up: // Pitch/rotate up
        if (!m_orbitalCameraMode) m_fpsCamera.rotate(0.0f, speedRotateDeg);
        if (m_orbitalCameraMode) m_orbitCamera.rotate(0.0f, speedRotateDeg);
        break;
    case Qt::Key_Down: // Pitch/rotate down
        if (!m_orbitalCameraMode) m_fpsCamera.rotate(0.0f, -speedRotateDeg);
        if (m_orbitalCameraMode) m_orbitCamera.rotate(0.0f, -speedRotateDeg);
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Statistics
///////////////////////////////////////////////////////////////////////////////

void GLWidget::initializeStatistics()
{
    connect(this, &QOpenGLWidget::aboutToCompose, this, &GLWidget::onAboutToCompose);
    connect(this, &QOpenGLWidget::frameSwapped, this, &GLWidget::onFrameSwapped);
    QTimer * timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, [=] {
        topLevelWidget()->setWindowTitle(QString("%1 - %2 fps, %3 ms / 1s").arg(MainWindow::APP_TITLE).arg(m_frameCount).arg(float(m_nsecsElapsed)/1000000, 3));
        m_frameCount = 0;
        m_nsecsElapsed = 0;
    });
    timer->start();
}

void GLWidget::onFrameSwapped()
{
    m_frameCount++;
    m_nsecsElapsed += m_elapsedTime.nsecsElapsed();
}

void GLWidget::onAboutToCompose()
{
    m_elapsedTime.restart();
}
