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

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
    , m_ibo(QOpenGLBuffer::IndexBuffer)
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
    cleanup();
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
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup, Qt::DirectConnection);

    qInfo() << "Initialize : Vertex Buffer Object (vbo)";
    // Set up an array of vertices for a quad (2 triangls)
    // with an index buffer data
    GLfloat vertices[] = {
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // Top left
         0.5f,  0.5f, 0.0f,	  1.0f, 1.0f,  // Top right
         0.5f, -0.5f, 0.0f,	  1.0f, 0.0f,  // Bottom right
        -0.5f, -0.5f, 0.0f,	  0.0f, 0.0f   // Bottom left
    };

    GLuint indices[] = {
        0, 1, 2,  // First Triangle
        0, 2, 3   // Second Triangle
    };

    // Request OpenGL to create new Vertex Array Object (VAO)
    // "bind" - make it the current one for following commands
    if (!m_vao.create()) {
        qWarning() << "Initialize : vbo failed!";
        return;
    }

    // bind and unbind when going out of scope
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Set up vertex buffer(s) on the GPU
    // GL_ARRAY_BUFFER (Vertex attributes)
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glBindBuffer.xhtml
    // Generate 1 empty vertex buffer on the GPU
    //
    // Copy the vertex data from CPU to GPU
    // STREAM
    //  The data store contents will be modified once and used at most a few times.
    // STATIC*
    //  The data store contents will be modified once and used many times.
    // DYNAMIC
    //  The data store contents will be modified repeatedly and used many times.
    //
    // StaticDraw is STATIC write only
    //
    // "bind" which means set as the current buffer the next command apply to
    //
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glBufferData.xhtml

    if (!m_vbo.create()) {
        qWarning() << "Initialize : vbo failed!";
        return;
    }
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(&vertices, sizeof(vertices));

    qInfo() << "Initialize : Vertex Array Object (vao)";

    // Define a simple layout for the first vertex buffer (index 0)
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glVertexAttribPointer.xhtml
    // 3 floats of data that should not be normalized (data is normalized to the viewport already)
    // Stride of 0 states that the data is tightly packed (3x4=12 bytes per vertex, stride is then 12)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));

    // Enable the first attribute or attribute index 0
    glEnableVertexAttribArray(0);

    // Same stride but offset is 3*3 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Set up index buffer which is used to indexed based vertex lookup
    // which reduces the number of vertices. Instead of 6, now we only need 4 vertices.
    qInfo() << "Initialize : Vertex Index Object (vao)";

    if (!m_ibo.create()) {
        qWarning() << "Initialize : ibo failed!";
        return;
    }
    m_ibo.bind();
    m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo.allocate(&indices, sizeof(indices));

    qInfo() << "Initialize : Shaders ";
    m_shaderProgram.loadShaders(":/Shaders/basictexture.vert",
                                ":/Shaders/basictexture.frag");

    m_texture.loadTexture(":/Images/funpic.jpg", true);

    qInfo() << "Initialize : DONE ... start the update timer";
    m_programStart = QTime::currentTime();
    m_timerId = startTimer(10);
    m_timerStarted = true;
}

void GLWidget::cleanup()
{
    qInfo() << "Shutdown : cleanup";

    makeCurrent();
    m_shaderProgram.unloadShaders();
    m_vbo.destroy();
    m_ibo.destroy();
    doneCurrent();

    // Disconnect to the current context
    QObject::disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
}
void GLWidget::paintGL()
{
    // NOTE: no logging here, this function is called very often

    // Clear the viewport
    glClearColor(m_background.redF(), m_background.greenF(), m_background.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT);

    m_texture.bind();

    // Render the rectangle (two triangles)
    // Must be called BEFORE setting uniforms because setting uniforms
    // is done on the currently active shader program.
    m_shaderProgram.use();

    // Just to demonstrate the rendering over time, add some movement
    // The shader program is responsible for translating and colouring
    // the object using the Uniforms data variables.

    // Time since app was started
    QTime programRun = QTime::currentTime();
    GLfloat timeSecs = GLfloat(m_programStart.msecsTo(programRun)) / 1000.0;

    // Color changes beween 0.0 and 1.0 depending on time
    // sin goes from -1.0 to +1.0, so divide by 2 and add 0.5
    GLfloat blueColor = (sin(timeSecs) / 2.0f) + 0.5f;
    m_shaderProgram.setUniform("vertColor", QVector4D(0.0f, 0.0f, blueColor, 1.0f));

    // One circle every pi() seconds. As the rectange has half the width and height of the viewport
    // we move in a radius of 0.5 of the normalized viewport, which goes from -1.0 to +1.0
    QVector2D pos;
    pos.setX( sin(timeSecs) / 2.0 );
    pos.setY( cos(timeSecs) / 2.0 );
    m_shaderProgram.setUniform("posOffset", pos);

    // Before we draw the vertices "bind" (select) to the vao first, unbind when object goes out of scope
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // The triangles will be drawn with this mode
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframeMode ? GL_LINE : GL_FILL);

    // Draw the "elements" - 0 offset
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // "unbind" to ensure no further changes the vao can be made
    glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////
/// UI handling
///////////////////////////////////////////////////////////////////////////////

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_F1:
        qInfo() << "Application - F1 - fullscreen toggle.";
        if (!window()->isFullScreen())
            window()->showFullScreen();
        else
            window()->showNormal();
        break;
    case Qt::Key_Escape:
        qInfo() << "Application - Escaping ... quit.";
        qApp->quit();
        break;
    case Qt::Key_W:
        m_wireframeMode = !m_wireframeMode;
        qInfo() << "Application - toggle wireframe mode." << m_wireframeMode;
        break;
    case Qt::Key_T:
        m_timerStarted = !m_timerStarted;
        if (m_timerStarted)
            m_timerId = startTimer(10);
        else
            killTimer(m_timerId);

        qInfo() << "Application - toggle timer." << m_timerStarted;
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
