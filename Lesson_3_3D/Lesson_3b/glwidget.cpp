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
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DRender/QMesh>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QCuboidGeometry>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_playerCamera(QVector3D(0.0f, 0.0f, 10.0f), QVector3D(0.0f, 0.0f, 0.0f))
    , m_orbitCamera(10.0f, 0.0f, 0.0f)
    , m_orbitalCameraMode(true)
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
    Qt3DExtras::QCuboidMesh * cubeVertices = new Qt3DExtras::QCuboidMesh();
    cubeVertices->setXExtent(2.0);
    cubeVertices->setYExtent(2.0);
    cubeVertices->setZExtent(2.0);

    // Qt3DCore::QEntity *cubeEntity1 = new Qt3DCore::QEntity();
    // cubeEntity1->setObjectName(QStringLiteral("Cube 1"));
    // cubeEntity1->addComponent(cubeVertices);

    qInfo() << "Cuboid mesh geometry: " << cubeVertices->geometry();
    qInfo() << "Cuboid mesh primitiveType: " << cubeVertices->primitiveType();
    qInfo() << "Cuboid mesh vertexCount: " << cubeVertices->vertexCount();
    Qt3DExtras::QCuboidGeometry * cubeGeometry = qobject_cast<Qt3DExtras::QCuboidGeometry *>(cubeVertices->view()->geometry());
    Q_ASSERT(cubeGeometry);
    qInfo() << "Cuboid vertices: " << cubeGeometry->positionAttribute()->name() << cubeGeometry->positionAttribute()->attributeType() << cubeGeometry->positionAttribute()->buffer();
    qInfo() << "Cuboid texCoord: " << cubeGeometry->texCoordAttribute()->name() << cubeGeometry->texCoordAttribute()->attributeType() << cubeGeometry->texCoordAttribute()->buffer();
    qInfo() << "Cuboid vertices buffersize: " << cubeGeometry->positionAttribute()->buffer()->data().size();
    qInfo() << "Cuboid texCoord buffersize: " << cubeGeometry->texCoordAttribute()->buffer()->data().size();

    int floatCount = cubeGeometry->positionAttribute()->buffer()->data().size() / 4;
    const float * floatPointer = reinterpret_cast<const float *>(cubeGeometry->positionAttribute()->buffer()->data().constData());
    int byteStride = cubeGeometry->positionAttribute()->byteStride();
    int floatStride = byteStride / 4;
    qInfo() << "Cuboid vertex stride (byte/float) : " << byteStride << floatStride;
    int tt = 1;
    for (int ii = 0; ii < floatCount; ii = ii + floatStride)
    {
        qInfo() << "vertex:" << tt++;
        qInfo() << " position x:" << floatPointer[ii ] << cubeGeometry->positionAttribute()->byteOffset();
        qInfo() << " position y:" << floatPointer[ii + 1];
        qInfo() << " position z:" << floatPointer[ii + 2];
        qInfo() << " texCoord x:" << floatPointer[ii + 3] << cubeGeometry->texCoordAttribute()->byteOffset();
        qInfo() << " texCoord y:" << floatPointer[ii + 4];
        qInfo() << " normal   x:" << floatPointer[ii + 5] << cubeGeometry->normalAttribute()->byteOffset();
        qInfo() << " normal   y:" << floatPointer[ii + 6];
        qInfo() << " normal   z:" << floatPointer[ii + 7];
        qInfo() << " tangent  x:" << floatPointer[ii + 8] << cubeGeometry->tangentAttribute()->byteOffset();
        qInfo() << " tangent  y:" << floatPointer[ii + 9];
        qInfo() << " tangent  z:" << floatPointer[ii + 10];
        qInfo() << " tangent  w:" << floatPointer[ii + 11];
    }
    int intCount = cubeGeometry->indexAttribute()->buffer()->data().size() / 2;
    const quint16 * indexPointer = reinterpret_cast<const quint16 *>(cubeGeometry->indexAttribute()->buffer()->data().constData());
    tt = 1;
    for (int ii = 0; ii < intCount; ii++)
    {
        qInfo() << "index: " << tt++ << ", value :" << indexPointer[ii];
    }

    // Cube and floor positions
    m_cubePos = QVector3D(0.0f, 0.0f, 0.0f);
    m_floorPos = QVector3D(0.0f, -1.0f, 0.0f);

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
    m_vbo.allocate(cubeGeometry->positionAttribute()->buffer()->data().constData(),
                   cubeGeometry->positionAttribute()->buffer()->data().size());

    qInfo() << "Initialize : Vertex Array Object (vao)";

    // Define a layout for the first vertex buffer (index 0)
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glVertexAttribPointer.xhtml
    // 3 floats of data that should not be normalized (data is normalized to the viewport already)
    // Stride of 5 floats (3x4=12 bytes per vertex + 2x4=8 bytes for texture position, stride is then 20)
    quint64 byteOffset = cubeGeometry->positionAttribute()->byteOffset();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byteStride, (GLvoid*)(byteOffset) );

    // Enable the first attribute or attribute index 0
    glEnableVertexAttribArray(0);

    // Same stride but offset is 3*3 floats
    byteOffset = cubeGeometry->texCoordAttribute()->byteOffset();
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, byteStride, (GLvoid*)(byteOffset) );
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
    m_ibo.allocate(cubeGeometry->indexAttribute()->buffer()->data().constData(), cubeGeometry->indexAttribute()->buffer()->data().size());

    // "unbind" is good to make sure other code doesn't change it elsewhere
    glBindVertexArray(0);

    delete cubeVertices;

    qInfo() << "Initialize : Shaders ";
    m_shaderProgram.loadShaders(":/Shaders/basictexture3D.vert",
                                ":/Shaders/basictexture3D.frag");

    m_texture.loadTexture(":/Images/funpic.jpg", true);
    m_textureFloor.loadTexture(":/Images/grid.jpg", true);

    // QMesh helper can be used to load and parse our 3D object file
    //Qt3DRender::QMesh *mesh = new Qt3DRender::QMesh();
    //mesh->setSource(QUrl(QStringLiteral("qrc:/object1.obj")));
    //qInfo() << "Initialize mesh: " << mesh->status();
    //mesh->dumpObjectTree();
    //mesh->dumpObjectInfo();

    //Qt3DCore::QEntity *object = new Qt3DCore::QEntity( /* rootEntity */ );
    //object->addComponent(mesh);

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
    m_texture.destroy();
    m_textureFloor.destroy();
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
    //model.scale(2.0, 1.0, 1.0);
    model.scale(1.0f + sinf(timeSecs) * 0.05f);
    // translate
    model.translate(m_cubePos);

    // Create the view matrix using the new camera class
    if (m_orbitalCameraMode)
    {
        view = m_orbitCamera.viewMatrix();
    }
    else
    {
        view = m_playerCamera.viewMatrix();
    }

    // Create the projection matrix
    //projection.setToIdentity();
    projection.perspective(m_playerCamera.getFOV(), width()/height(), 0.1f, 100.0f);

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
    m_vao.bind();

    // The triangles will be drawn with this mode
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframeMode ? GL_LINE : GL_FILL);

    // Draw the cube - 0 offset
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    // Position below the cube and squash it flat
    model.setToIdentity();
    model.translate(m_floorPos);
    model.scale(QVector3D(10.0f, 0.01f, 10.0f));

    // Update the M(VP) matrices inside the shaders
    m_shaderProgram.setUniform("model", model);

    m_textureFloor.bind();

    // Draw the floor using the same squashed cubeVertices
    //glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw the "elements" - 0 offset
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

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
    bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);

    ///////////////////
    // Move the cube
    ///////////////////
    if (shiftPressed)
    {
        switch(event->key())
        {
            case Qt::Key_Up:
            {
                m_cubePos += QVector3D(0.0f, speedMove, 0.0f);
                m_orbitCamera.setOrbitCenter(m_cubePos);
                break;
            }
            case Qt::Key_Down:
            {
                m_cubePos += QVector3D(0.0f, -speedMove, 0.0f );
                m_orbitCamera.setOrbitCenter(m_cubePos);
                break;
            }
            case Qt::Key_Left:
            {
                m_cubePos += QVector3D(-speedMove, 0.0f, 0.0f);
                m_orbitCamera.setOrbitCenter(m_cubePos);
                break;
            }
            case Qt::Key_Right:
            {
                m_cubePos += QVector3D(speedMove, 0.0f, 0.0f);
                m_orbitCamera.setOrbitCenter(m_cubePos);
                break;
            }
        }
        return;
    }

    ///////////////////
    // General functions
    ///////////////////
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
        m_playerCamera.setPosition(QVector3D(0.0f, 0.0f, 10.0f));
        m_playerCamera.setRotation(0.0f, 0.0f);
        m_orbitCamera.setRadius(10.0f);
        m_orbitCamera.setRotation(0.0f, 0.0f);
        qInfo() << "Application - toggle orbital camera mode." << m_orbitalCameraMode;
        break;
    }

    if (m_orbitalCameraMode)
    {
        ////////////////////////////
        // OrbitalCamera control
        ////////////////////////////
        switch(event->key())
        {
        case Qt::Key_W: // Camera forward (-z)
            m_orbitCamera.setRadius(m_orbitCamera.radius()-speedMove);
            break;
        case Qt::Key_S: // Camera backup (+z)
            m_orbitCamera.setRadius(m_orbitCamera.radius()+speedMove);
            break;
        case Qt::Key_A: // Not used
            break;
        case Qt::Key_D: // Not used
            break;
        case Qt::Key_L: // Camera move right
            m_orbitCamera.setLookAt(m_cubePos);
            break;
        case Qt::Key_Left: // Yaw to left (right hand rule, rotate around the y/up axis)
            m_orbitCamera.rotate(-speedRotateDeg, 0.0f);
            break;
        case Qt::Key_Right:// Yaw to right (right hand rule, rotate around the y/up axis)
            m_orbitCamera.rotate(speedRotateDeg, 0.0f);
            break;
        case Qt::Key_Up: // Pitch/rotate up
            m_orbitCamera.rotate(0.0f, speedRotateDeg);
            break;
        case Qt::Key_Down: // Pitch/rotate down
            m_orbitCamera.rotate(0.0f, -speedRotateDeg);
            break;
        }
    }
    else
    {
        ////////////////////////////
        // PlayerCamera control
        ////////////////////////////
        switch(event->key())
        {
        case Qt::Key_W:
        {
            // Camera forward (-z)
            auto look = m_playerCamera.lookVector();
            look *= speedMove;
            look.setY(0.0f); // stay on the ground
            m_playerCamera.move(look);
            break;
        }
        case Qt::Key_S:
        {
            // Camera backup (+z)
            auto look = m_playerCamera.lookVector();
            look *= -speedMove;
            look.setY(0.0f); // stay on the ground
            m_playerCamera.move(look);
            break;
        }
        case Qt::Key_A:
        {
            // Camera move left
            auto look = m_playerCamera.rightVector();
            look *= -speedMove;
            look.setY(0.0f); // stay on the ground
            m_playerCamera.move(look);
            break;
        }
        case Qt::Key_D:
        {
            // Camera move right
            auto look = m_playerCamera.rightVector();
            look *= speedMove;
            look.setY(0.0f); // stay on the ground
            m_playerCamera.move(look);
            break;
        }
        case Qt::Key_L: // Camera pitch and yaw set to look at cube
            m_playerCamera.setLookAt(m_cubePos);
            break;
        case Qt::Key_Left: // Yaw to left (right hand rule, rotate around the y/up axis)
            m_playerCamera.rotate(-speedRotateDeg, 0.0f);
            break;
        case Qt::Key_Right:// Yaw to right (right hand rule, rotate around the y/up axis)
            m_playerCamera.rotate(speedRotateDeg, 0.0f);
            break;
        case Qt::Key_Up: // Pitch up
            m_playerCamera.rotate(0.0f, speedRotateDeg);
            break;
        case Qt::Key_Down: // Pitch down
            m_playerCamera.rotate(0.0f, -speedRotateDeg);
            break;
        }
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
