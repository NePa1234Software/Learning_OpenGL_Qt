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

// Vertex input depends on the attib parameter list.
// 1st parameter is set with 3 x GL_FLOATS.
layout (location = 0) in vec3 pos;

// 2nd parameter is set with 2 x GL_FLOATS.
layout (location = 1) in vec2 texCoord;
out vec2 TexCoord;

// 3D MVP matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // gl_Position is the OpenGL built in variable which is passed to the fragment shader
    gl_Position = projection * view * model * vec4(pos, 1.0);
    TexCoord = texCoord;
}
