# Learning_OpenGL_Qt
Using modern OpenGL with the Qt framework. 

# Disclaimer/Acknowledgement
Note that I am NOT an expert at OpenGL. Instead I am learning through from the 
Udemy Course - https://www.udemy.com/course/learn-modern-opengl-programming by Steve Jones who is one. 
This Udemy course has inspired me to make this project and aims to demonstrate how Qt integrates OpenGL into its framework and which helper classes are provided.

# Lesson
- Lesson 3 - will set up the basic OpenGL pipeline to draw a 3D textured cube and demonstrate the vertex and fragment shader usage. Uniform variables are used to communicated between the application and the shader program.
- Lesson 3 a - will uses the lowest level OpenGL calls without any further Qt helper classes.
Only the minimum QOpenGLWidget and QOpenGLFunctions_3_3_Core classes are used.
- Lesson 3 b - will uses the the QOpenGLTexture to dramatically simplify the texture handling and QMesh helper classes to create our vertices for us.

# Code structure - Common to all lessons.
In main.cpp the default surface format is initialized and the OpenGL Core profile is enabled.
The GLWidget is set as the main widget by setting setCentralWidget.
GLWidget specialises a QOpenGLWidget class which provide the viewport and context for drawing, and the helper base class QOpenGLFunctions_3_3_Core to blend in all OpenGL functions of a specific OpenGL library version.

