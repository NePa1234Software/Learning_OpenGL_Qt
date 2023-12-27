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

#include <QOpenGLTexture>

class Texture2D : public QOpenGLTexture
{
public:
	Texture2D();
	virtual ~Texture2D();

    bool loadTexture(const QString & fileName, bool generateMipMaps = true);

    // Use QOpenGLTexture::bind
    // Release QOpenGLTexture::release
};
