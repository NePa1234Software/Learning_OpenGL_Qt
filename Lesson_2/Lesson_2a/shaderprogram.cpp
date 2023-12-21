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
    : m_Handle(0)
{
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_Handle);
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
    std::string vsStr = readFileToString(vsFilename);
    std::string fsStr = readFileToString(fsFilename);
    if (vsStr.empty()) return false;
    if (fsStr.empty()) return false;

    qInfo() << "Shader program : create shaders";
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    qInfo() << "Shader program : copy shader sources";
    const GLchar* vsStrPtr = vsStr.c_str();
    const GLchar* fsStrPtr = fsStr.c_str();
    glShaderSource(vs, 1, &vsStrPtr, NULL);
    glShaderSource(fs, 1, &fsStrPtr, NULL);

    qInfo() << "Shader program : compile shaders";
    glCompileShader(vs);
    if (!checkCompilationResult(vs, vsFilename)) return false;

    glCompileShader(fs);
    if (!checkCompilationResult(fs, fsFilename)) return false;

    qInfo() << "Shader program : create shader program";
    m_Handle = glCreateProgram();
    if (m_Handle == 0)
    {
        qWarning() << "Create shader program failed!";
		return false;
	}

    qInfo() << "Shader program : attach shaders to shader program";
    glAttachShader(m_Handle, vs);
    glAttachShader(m_Handle, fs);

    qInfo() << "Shader program : link the shader program";
    glLinkProgram(m_Handle);
    if (!checkLinkingResult(m_Handle))
        return false;

    // shaders are no longer needed
    glDeleteShader(vs);
	glDeleteShader(fs);

    // Ensure clean location lookup of all uniforms
    m_UniformLocations.clear();

    qInfo() << "Shader program : Ready";
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// Run
///////////////////////////////////////////////////////////////////////////////

void ShaderProgram::use()
{
    // As you can have multiple shader programs created you
    // need to tell OpenGL which program we want to use.
    if (m_Handle > 0)
        glUseProgram(m_Handle);
}

///////////////////////////////////////////////////////////////////////////////
/// Helper to read the shader file
///////////////////////////////////////////////////////////////////////////////

std::string ShaderProgram::readFileToString(const QString& filename)
{
    qInfo() << "Shader program : read - " << filename;
    QFile file(filename);
    if (!file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text))
    {
        qWarning() << "Shader program : File open FAILED - " << filename;
        return std::string();
    }
    qDebug() << "Shader program : File open OK - " << filename;
    QByteArray content = file.readAll();
    return content.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
/// Check and log the compilation result
///////////////////////////////////////////////////////////////////////////////

bool ShaderProgram::checkLinkingResult(GLuint shader)
{
    // https://registry.khronos.org/OpenGL-Refpages/es3/html/glGetProgramiv.xhtml
	int status = 0;
    glGetProgramiv(m_Handle, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length = 0;
        glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &length);

        // The length includes the NULL character
        // Resize and fill with space character
        std::string errorLog(length, ' ');
        glGetProgramInfoLog(m_Handle, length, &length, &errorLog[0]);
        qWarning() << "Shader program : failed to link. " << errorLog;
        return false;
    }
    return true;
}

bool ShaderProgram::checkCompilationResult(GLuint shader, const QString & filename)
{
    int status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        // The length includes the NULL character
        std::string errorLog(length, ' ');  // Resize and fill with space character
        glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
        qWarning() << "Shader program : failed to compile " << filename << ". " << errorLog;
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// Uniform access
///////////////////////////////////////////////////////////////////////////////

void ShaderProgram::setUniform(const GLchar* name, const QVector2D & v)
{
    GLint loc = getUniformLocation(name);
    glUniform2f(loc, v.x(), v.y());
}

void ShaderProgram::setUniform(const GLchar* name, const QVector3D & v)
{
    GLint loc = getUniformLocation(name);
    glUniform3f(loc, v.x(), v.y(), v.z());
}

void ShaderProgram::setUniform(const GLchar* name, const QVector4D & v)
{
    GLint loc = getUniformLocation(name);
    glUniform4f(loc, v.x(), v.y(), v.z(), v.w());
}

GLint ShaderProgram::getUniformLocation(const GLchar* name)
{
    QString qStr(name);
    if (qStr.isEmpty())
        return 0;

    // Only look up once and keep the location (improve performance)
    QMap<QString, GLint>::iterator it = m_UniformLocations.find(qStr);
    if (it == m_UniformLocations.end())
	{
        m_UniformLocations[qStr] = glGetUniformLocation(m_Handle, name);
	}
    return m_UniformLocations[qStr];
}
