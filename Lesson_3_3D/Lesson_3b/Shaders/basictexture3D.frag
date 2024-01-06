#version 330 core

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

in vec2 TexCoord;

// Color of the fragment (texel)
out vec4 frag_color;

uniform sampler2D texSampler;

void main()
{
    frag_color = texture(texSampler, TexCoord);
}
