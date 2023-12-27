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
#include <QOpenGLTexture>

Texture2D::Texture2D()
    : QOpenGLTexture(QOpenGLTexture::Target2D)
{
}

Texture2D::~Texture2D()
{
    destroy();
}

bool Texture2D::loadTexture(const QString & texFile, bool generateMipMaps)
{
    qInfo() << "Texture 2D : read texture file... ";

    if (generateMipMaps)
    {
        setData(QImage(texFile).mirrored(), QOpenGLTexture::GenerateMipMaps);
        Q_ASSERT(isAutoMipMapGenerationEnabled());
    }
    else
    {
        setData(QImage(texFile).mirrored(), QOpenGLTexture::DontGenerateMipMaps);
        Q_ASSERT(!isAutoMipMapGenerationEnabled());
    }
    setMinificationFilter(QOpenGLTexture::Linear);
    setMagnificationFilter(QOpenGLTexture::Linear);
    setWrapMode(QOpenGLTexture::Repeat);

    bool result = create();

    if (!result)
    {
        qWarning() << "Texture 2D : read texture file ... FAILED";
        return false;
    }
    Q_ASSERT(isStorageAllocated());
    qInfo() << "Texture 2D : texture file loaded ... " << format() << width() << height()<< depth() << levelOfDetailRange();
	return true;
}
