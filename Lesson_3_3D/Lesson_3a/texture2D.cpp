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
#include "texture2D.h"

#include <QImage>
#include <QImageReader>

Texture2D::Texture2D()
    : m_Texture(0)
{
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_Texture);
}

void Texture2D::initializeGL()
{
    initializeOpenGLFunctions();
}

bool Texture2D::loadTexture(const QString & texFile, bool generateMipMaps)
{
    initializeGL();

    qInfo() << "Texture 2D : read texture file... ";
    QImageReader texReader(texFile);
    QImage texData;
    bool result = texReader.read(&texData);
    if (!result)
    {
        qWarning() << "Texture 2D : read texture file ... FAILED, " << texReader.errorString();
        return false;
    }
    qInfo() << "Texture 2D : texture file loaded ... " << texData.format() << texData.width() << texData.height();
    Q_ASSERT(texData.bits() != nullptr);
    Q_ASSERT(texData.format() == QImage::Format_RGB32);
    Q_ASSERT(texData.depth() == 32);
    //Q_ASSERT(texData.bitPlaneCount() == 24);
    //Q_ASSERT(texData.width() == 1080);
    //Q_ASSERT(texData.height() == 1362);
    //Q_ASSERT(texData.bytesPerLine() == 1080*4);
    //Q_ASSERT(texData.sizeInBytes() == 1080*1362*4);
    if (texData.format() != QImage::Format_RGB32)
    {
        qWarning() << "Texture 2D : read texture file ... wrong format (not RGB32)";
        return false;
    }

    // Map to OpenGL coordinates (flip vertical)
    texData.mirror(false,true);

    // next GL_TEXTURE_2D operations will affect texture object
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	// GL_CLAMP_TO_EDGE
	// GL_REPEAT
	// GL_MIRRORED_REPEAT
	// GL_CLAMP_TO_BORDER
	// GL_LINEAR
	// GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texData.width(), texData.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.bits());

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0); // unbind texture when done so we don't accidentally mess up our m_Texture

	return true;
}

void Texture2D::bind(GLuint textureUnit)
{
    Q_ASSERT(textureUnit >= 0 && textureUnit < 32);
    if (!(textureUnit >= 0 && textureUnit < 32)) return;

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}
