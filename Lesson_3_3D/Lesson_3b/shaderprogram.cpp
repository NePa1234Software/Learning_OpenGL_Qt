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

#include <QFile>
#include <QString>
#include <QDebug>

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
    unloadShaders();
}

void ShaderProgram::initializeGL()
{
    initializeOpenGLFunctions();
}

///////////////////////////////////////////////////////////////////////////////
/// Load and compile
///////////////////////////////////////////////////////////////////////////////

bool ShaderProgram::loadShaders(const QString & vsFilename, const QString & fsFilename)
{
    initializeGL();

    qInfo() << "Shader program : read files... ";
    QString vsStr = readFileToString(vsFilename);
    QString fsStr = readFileToString(fsFilename);
    if (vsStr.isEmpty()) return false;
    if (fsStr.isEmpty()) return false;

    qInfo() << "Shader program : create shaders";
    m_program = new QOpenGLShaderProgram;

    qInfo() << "Shader program : copy and compile vertex shader sources";
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsStr))
    {
        qWarning() << "Shader program : failed to compile. " << m_program->log();
        return false;
    }

    qInfo() << "Shader program : copy and compile fragment shader sources";
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsStr))
    {
        qWarning() << "Shader program : failed to compile. " << m_program->log();
        return false;
    }

    qInfo() << "Shader program : link the shader program";
    if (!m_program->link())
    {
        qWarning() << "Shader program : failed to link. " << m_program->log();
        return false;
    }

    m_program->bind();

    // Ensure clean location lookup of all uniforms
    m_UniformLocations.clear();

    qInfo() << "Shader program : Ready";
    return true;
}

void ShaderProgram::unloadShaders()
{
    if (m_program)
        delete m_program;
    m_program = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
/// Run
///////////////////////////////////////////////////////////////////////////////

void ShaderProgram::use()
{
    // As you can have multiple shader programs created you
    // need to tell OpenGL which program we want to use.
    if (m_program)
        m_program->bind();
}

void ShaderProgram::release()
{
    if (m_program)
        m_program->release();
}

///////////////////////////////////////////////////////////////////////////////
/// Helper to read the shader file
///////////////////////////////////////////////////////////////////////////////

QString ShaderProgram::readFileToString(const QString& filename)
{
    qInfo() << "Shader program : read - " << filename;
    QFile file(filename);
    if (!file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text))
    {
        qWarning() << "Shader program : File open FAILED - " << filename;
        return QString();
    }
    qDebug() << "Shader program : File open OK - " << filename;
    QByteArray content = file.readAll();
    QString retStr(content);
    return retStr;
}

///////////////////////////////////////////////////////////////////////////////
/// Uniform access
///////////////////////////////////////////////////////////////////////////////

void ShaderProgram::setUniform(const GLchar* name, const QVector2D & v)
{
    if (!m_program) return;
    int loc = getUniformLocation(name);
    m_program->setUniformValue(loc, v);
}

void ShaderProgram::setUniform(const GLchar* name, const QVector3D & v)
{
    if (!m_program) return;
    int loc = m_program->uniformLocation(name);
    m_program->setUniformValue(loc, v);
}

void ShaderProgram::setUniform(const GLchar* name, const QVector4D & v)
{
    if (!m_program) return;
    int loc = getUniformLocation(name);
    m_program->setUniformValue(loc, v);
}

void ShaderProgram::setUniform(const GLchar *name, const QMatrix4x4 &m)
{
    if (!m_program) return;
    GLint loc = getUniformLocation(name);
    // constData is column major data, thus no transpose needed
    m_program->setUniformValue(loc, m);
    //m_program->glUniformMatrix4fv(loc, 1, GL_FALSE, m.constData());
}

int ShaderProgram::getUniformLocation(const GLchar* name)
{
    if (!m_program) return -1;

    QString qStr(name);
    if (qStr.isEmpty())
        return -1;

    // Only look up once and keep the location (improve performance)
    QMap<QString, GLint>::iterator it = m_UniformLocations.find(qStr);
    if (it == m_UniformLocations.end())
    {
        int result = m_program->uniformLocation(name);
        if (result == -1)
        {
            qWarning() << "Shader program : uniform location lookup FAILED - " << name;
        }
        m_UniformLocations[qStr] = result;
    }
    return m_UniformLocations[qStr];
}
