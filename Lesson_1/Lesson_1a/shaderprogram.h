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

#include <QFile>
#include <QOpenGLFunctions>
#include <QString>
#include <QMap>

// For Qt version of vec/mat types see
// https://doc.qt.io/qt-6/qml-qtquick-shadereffect.html
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLFunctions_3_3_Core>

///
/// \brief The ShaderProgram class uses the OpenGL Core 330 Wrapper which
/// opens up all functions for this specific library version when the
/// initializeOpenGLFunctions function is called. Note the surface settings
/// in main.cpp to enable we core profile.
/// Only call loadShaders when the OpenGL is already set, e.g. from
/// QOpenGLWidget::initializeGL
///
class ShaderProgram: public QOpenGLFunctions_3_3_Core // QOpenGLFunctions
{
public:
	 ShaderProgram();
	~ShaderProgram();

    // Path is relative to application or absolute, or best use qrc url
    // This initializes the QOpenGLFunctions for the current OpenGL context
    bool loadShaders(const QString & vsFilename, const QString & fsFilename);

    // Apply this shader program
    void use();

    GLuint getProgram() const
    {
        return m_Handle;
    }

    // Set the uniform by name
    void setUniform(const GLchar* name, const QVector2D & v);
    void setUniform(const GLchar* name, const QVector3D & v);
    void setUniform(const GLchar* name, const QVector4D & v);

private:

    void initializeGL();

    // Read the file into a string. String is empty on failure and error logged
    std::string readFileToString(const QString & filename);

    // Return false if compilation errors occured. Errors are logged
    bool checkCompilationResult(GLuint shader);
    bool checkLinkingResult(GLuint shader);

    // Find and keep location to the uniform by exact name
	GLint getUniformLocation(const GLchar * name);

    GLuint m_Handle {0};
    QMap<QString, GLint> m_UniformLocations;
    QOpenGLContext *m_context {nullptr};
};
