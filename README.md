# Learning_OpenGL_Qt
Using modern OpenGL with the Qt framework using the [OpenGL module](https://doc.qt.io/qt-6/qtopengl-index.html). 

# Disclaimer/Acknowledgement
Note that I am NOT an expert at OpenGL. Instead I am learning through from the 
Udemy Course - https://www.udemy.com/course/learn-modern-opengl-programming by Steve Jones who is one. This Udemy course has inspired me to make this project and aims to demonstrate how Qt integrates OpenGL into its framework and which helper classes are provided.

# Lesson
- Lesson 1 - will set up the basic OpenGL pipeline and demonstrate the vertex and fragment shader usage. Uniform variables are used to communicated between the application and the shader program.
- Lesson 2 - build upon lesson 1 adding an image based 2D texture.
- Lesson 3 - 3D objects and texturing. Matrices - model, view and perspective matrices. Translation, Rotation and Scaling (Model matrix = TRS matrix)

# Code structure - Common to all lessons.
In main.cpp the default surface format is initialized and the OpenGL Core profile is enabled.
The GLWidget is set as the main widget by setting setCentralWidget.
GLWidget specialises a QOpenGLWidget class which provide the viewport and context for drawing, and the helper base class QOpenGLFunctions_3_3_Core to blend in all OpenGL functions of a specific OpenGL library version.
