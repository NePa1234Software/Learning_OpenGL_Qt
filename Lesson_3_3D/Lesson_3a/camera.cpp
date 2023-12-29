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
#include "camera.h"

#include <QtMath>
#include <QDebug>

//------------------------------------------------------------
// Base Camera class constructor
//------------------------------------------------------------
ICamera::ICamera()
: m_Position(QVector3D(0.0f, 0.0f, 10.0f)),
  m_TargetPosition(QVector3D(0.0f, 0.0f, 0.0f)),
  m_Up(QVector3D(0.0f, 1.0f, 0.0f)),
  m_Right(0.0f, 0.0f, 0.0f),
  WORLD_UP(0.0f, 1.0f, 0.0f),
  m_YawDeg(180.0f),
  m_PitchDeg(0.0f),
  m_FovDegrees(DEF_FOV)
{
}

void ICamera::setViewMatrix(const QMatrix4x4 &view)
{
    if (view == m_viewMatrix)
        return;
    m_viewMatrix = view;
    qDebug() << "ICamera - view changed :" << view;
}

//------------------------------------------------------------
// Base Camera - Returns view matrix
//------------------------------------------------------------
void ICamera::calcViewMatrix()
{
    QMatrix4x4 vMatrix;
    vMatrix.lookAt(m_Position, m_TargetPosition, m_Up);
    setViewMatrix(vMatrix);
}

//------------------------------------------------------------
// Base Camera - Returns camera's local look vector
//------------------------------------------------------------
const QVector3D& ICamera::getLook() const
{
    return m_Look;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local right vector
//------------------------------------------------------------
const QVector3D& ICamera::getRight() const
{
    return m_Right;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local up vector
//------------------------------------------------------------
const QVector3D& ICamera::getUp() const
{
    return m_Up;
}

//-----------------------------------------------------------------------------
// FPSCamera - Constructor
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera(QVector3D position, float yawDegrees, float pitchDegrees)
{
    m_Position = position;
    m_YawDeg = yawDegrees;
    m_PitchDeg = pitchDegrees;

    updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Constructor 
// Given camera staring point and starting target point to look at
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera(QVector3D position, QVector3D target)
{
    m_Position = position;
    setLookAt(target);
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the camera position in world space
//-----------------------------------------------------------------------------
void FPSCamera::setPosition(const QVector3D& position)
{
    m_Position = position;
    updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the incremental position of the camera in world space
//-----------------------------------------------------------------------------
void FPSCamera::move(const QVector3D& offsetPos)
{
    m_Position += offsetPos;
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the incremental orientation of the camera
//-----------------------------------------------------------------------------
void FPSCamera::rotate(float yawDegrees, float pitchDegrees)
{
    setRotation(yawDegrees + m_YawDeg, pitchDegrees + m_PitchDeg);
}

void FPSCamera::setRotation(float yawDegrees, float pitchDegrees)
{
    // Wrap around and keep range 0 to 360 degrees
    while (yawDegrees >= 360.0f)
        yawDegrees -= 360.0f;
    while (yawDegrees < 0.0)
        yawDegrees += 360.0f;

    // Constrain the pitch
    pitchDegrees = qBound(-89.9f, pitchDegrees, 89.9f);

    // Check for no change
    if (qFuzzyCompare(m_PitchDeg, pitchDegrees) && qFuzzyCompare(m_YawDeg, yawDegrees) )
        return;

    // Update the matrices
    m_YawDeg = yawDegrees;
    m_PitchDeg = pitchDegrees;
    updateCameraVectors();
}

void FPSCamera::setLookAt(const QVector3D& target)
{
    if (m_TargetPosition == target)
        return;

    m_TargetPosition = target;

    // Calculate the vector that looks at the target from the camera position
    QVector3D lookDir = position() - target;

    // Now Calculate the pitch and yaw from the target look vector.  (radians)
    m_PitchDeg = qRadiansToDegrees(-atan2(lookDir.y(), sqrt(lookDir.x() * lookDir.x() + lookDir.z() * lookDir.z())));
    m_YawDeg = qRadiansToDegrees(atan2(lookDir.x(), lookDir.z()));

    updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Calculates the front vector from the Camera's (updated) Euler Angles
//-----------------------------------------------------------------------------
void FPSCamera::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system (NOTE: Our coordinate sys has Y up not Z)

	// Calculate the view direction vector based on yaw and pitch angles (roll not considered)
	// radius is 1 for normalized length

    QVector3D look;
    look.setX(cosf(qDegreesToRadians(m_PitchDeg)) * sinf(qDegreesToRadians(m_YawDeg)));
    look.setY(sinf(qDegreesToRadians(m_PitchDeg)));
    look.setZ(cosf(qDegreesToRadians(m_PitchDeg)) * cosf(qDegreesToRadians(m_YawDeg + 180.0f)));
    m_Look = look.normalized();

	// Re-calculate the Right and Up vector.  For simplicity the Right vector will
    // be assumed horizontal w.r.t. the world's Up vector.
    m_Right = QVector3D::crossProduct(m_Look, WORLD_UP).normalized();
    m_Up = QVector3D::crossProduct(m_Right, m_Look).normalized();

    m_TargetPosition = m_Position + m_Look * m_Position.length();

    QMatrix4x4 vMatrix;
    vMatrix.lookAt(m_Position, m_TargetPosition, m_Up);
    setViewMatrix(vMatrix);

    qDebug() << QString("FPSCamera - Pitch:%1, Yaw:%2").arg(m_PitchDeg).arg(m_YawDeg);
    qDebug() << "FPSCamera - Pos:" << m_Position << ", Target:" << m_TargetPosition;
    qDebug() << "FPSCamera - Look:" << m_Look << ", Right:" << m_Right << ", Up:" << m_Up;
    calcViewMatrix();
}

//------------------------------------------------------------
// OrbitCamera - constructor
//------------------------------------------------------------
OrbitCamera::OrbitCamera(float radius, float yawDegrees, float pitchDegrees)
{
    setRadius(radius);
    setRotation(yawDegrees, pitchDegrees);
}


OrbitCamera::OrbitCamera()
{
    setRadius(10.0f);
    setRotation(0.0f, 0.0f);
}

//------------------------------------------------------------
// OrbitCamera - Sets the target to look at
//------------------------------------------------------------
void OrbitCamera::setLookAt(const QVector3D& target)
{
    m_TargetPosition = target;

    // Calculate the vector that looks at the target from the camera position
    QVector3D lookDir = position() - target;

    m_Radius = lookDir.length();

    // Now Calculate the pitch and yaw from the target look vector.  (radians)
    m_PitchDeg = qRadiansToDegrees(-atan2(lookDir.y(), sqrt(lookDir.x() * lookDir.x() + lookDir.z() * lookDir.z())));
    m_YawDeg = qRadiansToDegrees(atan2(lookDir.x(), lookDir.z()));

    updateCameraVectors();
}

//------------------------------------------------------------
// OrbitCamera - Sets the radius of camera to target distance
//------------------------------------------------------------
void OrbitCamera::setRadius(float radius)
{
	// Clamp the radius
    radius = qBound(1.0f, radius, 100.0f);

    if (qFuzzyCompare(m_Radius, radius))
        return;

    m_Radius = radius;

    updateCameraVectors();
}

//------------------------------------------------------------
// OrbitCamera - Rotates the camera around the target look
// at position given yaw and pitch in degrees.
//------------------------------------------------------------
void OrbitCamera::rotate(float yawDegrees, float pitchDegrees)
{
    setRotation(yawDegrees + m_YawDeg, pitchDegrees + m_PitchDeg);
}

void OrbitCamera::setRotation(float yawDegrees, float pitchDegrees)
{
    // Wrap around and keep range 0 to 360 degrees
    while (yawDegrees >= 360.0f)
        yawDegrees -= 360.0f;
    while (yawDegrees < 0.0)
        yawDegrees += 360.0f;

    // Constrain the pitch
    pitchDegrees = qBound(-89.9f, pitchDegrees, 89.9f);

    // Check for no change
    if (qFuzzyCompare(m_PitchDeg, pitchDegrees) && qFuzzyCompare(m_YawDeg, yawDegrees) )
        return;

    // Update the matrices
    m_YawDeg = yawDegrees;
    m_PitchDeg = pitchDegrees;
    updateCameraVectors();
}

//------------------------------------------------------------
// OrbitCamera - Calculates the front vector from the Camera's
// (updated) Euler Angles
//------------------------------------------------------------
void OrbitCamera::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
    // https://en.wikipedia.org/wiki/Spherical_coordinate_system (NOTE: Our coordinate sys has Y up not Z)
    m_Position.setX(m_TargetPosition.x() + m_Radius * cosf(qDegreesToRadians(m_PitchDeg)) * sinf(qDegreesToRadians(m_YawDeg)));
    m_Position.setY(m_TargetPosition.y() + m_Radius * sinf(qDegreesToRadians(m_PitchDeg)));
    m_Position.setZ(m_TargetPosition.z() + m_Radius * cosf(qDegreesToRadians(m_PitchDeg)) * cosf(qDegreesToRadians(m_YawDeg)));

    qDebug() << QString("OrbitCamera - Radius: %1, Pitch:%2, Yaw:%3").arg(m_Radius).arg(m_PitchDeg).arg(m_YawDeg);
    qDebug() << "OrbitCamera - Pos:" << m_Position << ", Target:" << m_TargetPosition;
    //qDebug() << "OrbitCamera - Look:" << m_Look << ", Right:" << m_Right << ", Up:" << m_Up;
    calcViewMatrix();
}
