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

// Default camera values
const float DEF_FOV = 45.0f; // degrees

//------------------------------------------------------------
// Base Camera class constructor
//------------------------------------------------------------
ICamera::ICamera()
: m_Position(QVector3D(0.0f, 0.0f, 10.0f)),
  m_TargetPos(QVector3D(0.0f, 0.0f, 0.0f)),
  m_Up(QVector3D(0.0f, 1.0f, 0.0f)),
  m_Right(0.0f, 0.0f, 0.0f),
  WORLD_UP(0.0f, 1.0f, 0.0f),
  m_YawDeg(180.0f),
  m_PitchDeg(0.0f),
  m_FovDegrees(DEF_FOV)
{
}

//------------------------------------------------------------
// Base Camera - Returns view matrix
//------------------------------------------------------------
QMatrix4x4 ICamera::getViewMatrix()const
{
    QMatrix4x4 vMatrix;
    vMatrix.lookAt(m_Position, m_TargetPos, m_Up);
    return vMatrix;
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
}

//-----------------------------------------------------------------------------
// FPSCamera - Constructor 
// Given camera staring point and starting target point to look at
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera(QVector3D position, QVector3D target)
{
    m_Position = position;
    m_TargetPos = target;

	// Calculate the vector that looks at the target from the camera position
    QVector3D lookDir = position - target;

    // Now Calculate the pitch and yaw from the target look vector.  (radians)
    m_PitchDeg = qRadiansToDegrees(-atan2(lookDir.y(), sqrt(lookDir.x() * lookDir.x() + lookDir.z() * lookDir.z())));
    m_YawDeg = qRadiansToDegrees(atan2(lookDir.x(), lookDir.z())) + 180.0;

    updateCameraVectors();
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
    m_YawDeg += yawDegrees;
    m_PitchDeg += pitchDegrees;
    setRotation(m_YawDeg, m_PitchDeg);
}

void FPSCamera::setRotation(float yawDegrees, float pitchDegrees)
{
    m_YawDeg = yawDegrees;
    m_PitchDeg = pitchDegrees;

    // Constrain the pitch
    m_PitchDeg = qBound(-89.9f, m_PitchDeg, 89.9f);

    // Wrap around and keep range 0 to 360 degrees
    if (m_YawDeg > 360.0f)
        m_YawDeg -= 360.0f;
    if (m_YawDeg < 0.0)
        m_YawDeg += 360.0f;
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
    look.setZ(cosf(qDegreesToRadians(m_PitchDeg)) * cosf(qDegreesToRadians(m_YawDeg)));

    m_Look = look.normalized();

	// Re-calculate the Right and Up vector.  For simplicity the Right vector will
    // be assumed horizontal w.r.t. the world's Up vector.
    m_Right = QVector3D::crossProduct(m_Look, WORLD_UP).normalized();
    m_Up = QVector3D::crossProduct(m_Right, m_Look).normalized();

    m_TargetPos = m_Position + m_Look;
}

//------------------------------------------------------------
// OrbitCamera - constructor
//------------------------------------------------------------
OrbitCamera::OrbitCamera()
    : m_Radius(10.0f)
{}

//------------------------------------------------------------
// OrbitCamera - Sets the target to look at
//------------------------------------------------------------
void OrbitCamera::setLookAt(const QVector3D& target)
{
    m_TargetPos = target;

    // Calculate the vector that looks at the target from the camera position
    QVector3D lookDir = position() - target;

    // Now Calculate the pitch and yaw from the target look vector.  (radians)
    m_PitchDeg = qRadiansToDegrees(-atan2(lookDir.y(), sqrt(lookDir.x() * lookDir.x() + lookDir.z() * lookDir.z())));
    m_YawDeg = qRadiansToDegrees(atan2(lookDir.x(), lookDir.z())) + 180.0;

    updateCameraVectors();
}

//------------------------------------------------------------
// OrbitCamera - Sets the radius of camera to target distance
//------------------------------------------------------------
void OrbitCamera::setRadius(float radius)
{
	// Clamp the radius
    m_Radius = qBound(radius, 2.0f, 80.0f);
    updateCameraVectors();
}

//------------------------------------------------------------
// OrbitCamera - Rotates the camera around the target look
// at position given yaw and pitch in degrees.
//------------------------------------------------------------
void OrbitCamera::rotate(float yawDegrees, float pitchDegrees)
{
    m_YawDeg += yawDegrees;
    m_PitchDeg += pitchDegrees;
    setRotation(m_YawDeg, m_PitchDeg);
}

void OrbitCamera::setRotation(float yawDegrees, float pitchDegrees)
{
    m_YawDeg = yawDegrees;
    m_PitchDeg = pitchDegrees;

    // Constrain the pitch
    m_PitchDeg = qBound(-89.9f, m_PitchDeg, 89.9f);

    // Wrap around and keep range 0 to 360 degrees
    if (m_YawDeg > 360.0f)
        m_YawDeg -= 360.0f;
    if (m_YawDeg < 0.0)
        m_YawDeg += 360.0f;

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
    m_Position.setX(m_TargetPos.x() + m_Radius * cosf(qDegreesToRadians(m_PitchDeg)) * sinf(qDegreesToRadians(m_YawDeg)));
    m_Position.setY(m_TargetPos.y() + m_Radius * sinf(qDegreesToRadians(m_PitchDeg)));
    m_Position.setZ(m_TargetPos.z() + m_Radius * cosf(qDegreesToRadians(m_PitchDeg)) * cosf(qDegreesToRadians(m_YawDeg)));
}
