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

// Vertex input depends on the attib parameter list. Only one pareter is set with 3 x GL_FLOATS.
layout (location = 0) in vec3 pos;

// Transform offset set by the application on each update
uniform vec2 posOffset;

void main()
{
    // gl_Position is the OpenGL built in variable which is passed to the fragment shader
    gl_Position = vec4(pos.x + posOffset.x, pos.y + posOffset.y, pos.z, 1.0);
}
