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

//-----------------------------------------------------------------------------
// ICamera base class
//-----------------------------------------------------------------------------
ICamera::ICamera()
: m_Position(QVector3D(0.0f, 0.0f, 10.0f)),
  m_TargetPosition(QVector3D(0.0f, 0.0f, 0.0f)),
  m_Up(QVector3D(0.0f, 1.0f, 0.0f)),
  m_Right(0.0f, 0.0f, 0.0f),
  WORLD_UP(0.0f, 1.0f, 0.0f),
  m_YawDeg(0.0f),
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

void ICamera::calcViewMatrix()
{
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(m_Position, m_TargetPosition, m_Up);
    setViewMatrix(viewMatrix);
}

void ICamera::setTargetPosition(const QVector3D &targetPosition)
{
    setLookAt(targetPosition);
}

void ICamera::setPosition(const QVector3D& position)
{
    m_Position = position;
    updateCameraVectors();
}

void ICamera::move(const QVector3D& offsetPos)
{
    m_Position += offsetPos;
    updateCameraVectors();
}

void ICamera::setLookAt(const QVector3D& target)
{
    if (m_TargetPosition == target)
        return;

    m_TargetPosition = target;

    // Calculate the vector that looks at the target from the camera position
    QVector3D lookDir = target - position();

    // Calculate the pitch and yaw from the target look vector
    // yaw 0 degrees is looking down -z, so add 180 degrees
    float pitchDeg = qRadiansToDegrees(-atan2(lookDir.y(), sqrt(lookDir.x() * lookDir.x() + lookDir.z() * lookDir.z())));
    float yawDeg = qRadiansToDegrees(atan2(lookDir.x(), lookDir.z()));
    yawDeg += 180.0f;
    setRotation(yawDeg, pitchDeg);

    updateCameraVectors();
}

void ICamera::rotate(float yawDegrees, float pitchDegrees)
{
    setRotation(yawDegrees + m_YawDeg, pitchDegrees + m_PitchDeg);
}

void ICamera::setRotation(float yawDegrees, float pitchDegrees)
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

const QVector3D& ICamera::lookVector() const
{
    return m_Look;
}

const QVector3D& ICamera::rightVector() const
{
    return m_Right;
}

const QVector3D& ICamera::upVector() const
{
    return m_Up;
}

//-----------------------------------------------------------------------------
// PlayerCamera
//-----------------------------------------------------------------------------
PlayerCamera::PlayerCamera(QVector3D position, float yawDegrees, float pitchDegrees)
{
    m_Position = position;
    m_YawDeg = yawDegrees;
    m_PitchDeg = pitchDegrees;
    updateCameraVectors();
}

PlayerCamera::PlayerCamera(QVector3D position, QVector3D target)
{
    m_Position = position;
    setLookAt(target);
    updateCameraVectors();
}

void PlayerCamera::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
    // https://en.wikipedia.org/wiki/Spherical_coordinate_system
    QVector3D look;
    look.setX(cosf(qDegreesToRadians(m_PitchDeg)) * sinf(qDegreesToRadians(m_YawDeg)));
    look.setY(sinf(qDegreesToRadians(m_PitchDeg)));
    look.setZ(cosf(qDegreesToRadians(m_PitchDeg)) * cosf(qDegreesToRadians(m_YawDeg + 180.0f)));

    m_Look = look.normalized();
    m_Right = QVector3D::crossProduct(m_Look, WORLD_UP).normalized();
    m_Up = QVector3D::crossProduct(m_Right, m_Look).normalized();

    m_TargetPosition = m_Position + m_Look * m_Position.length();

    QMatrix4x4 vMatrix;
    vMatrix.lookAt(m_Position, m_TargetPosition, m_Up);
    setViewMatrix(vMatrix);

    qDebug() << QString("PlayerCamera - Pitch:%1, Yaw:%2").arg(m_PitchDeg).arg(m_YawDeg);
    qDebug() << "PlayerCamera - Pos:" << m_Position << ", Target:" << m_TargetPosition;
    qDebug() << "PlayerCamera - Look:" << m_Look << ", Right:" << m_Right << ", Up:" << m_Up;
    calcViewMatrix();
}

//------------------------------------------------------------
// OrbitCamera
//------------------------------------------------------------
OrbitCamera::OrbitCamera(float radius, float yawDegrees, float pitchDegrees)
{
    setRadius(radius);
    setRotation(yawDegrees, pitchDegrees);
    setTargetPosition(QVector3D(0.0f,0.0f,0.0f));
    updateCameraVectors();
}


OrbitCamera::OrbitCamera()
{
    setRadius(10.0f);
    setRotation(0.0f, 0.0f);
    setTargetPosition(QVector3D(0.0f,0.0f,0.0f));
    updateCameraVectors();
}

void OrbitCamera::setRadius(float radius)
{
	// Clamp the radius
    radius = qBound(1.0f, radius, 100.0f);

    if (qFuzzyCompare(m_Radius, radius))
        return;

    m_Radius = radius;

    updateCameraVectors();
}

void OrbitCamera::updateCameraVectors()
{
    // Spherical to Cartesian coordinates from the Euler angles pitch and yaw
    // https://en.wikipedia.org/wiki/Spherical_coordinate_system
    m_Position.setX(m_Radius * cosf(qDegreesToRadians(m_PitchDeg)) * sinf(qDegreesToRadians(m_YawDeg)));
    m_Position.setY(m_Radius * sinf(qDegreesToRadians(m_PitchDeg)));
    m_Position.setZ(m_Radius * cosf(qDegreesToRadians(m_PitchDeg)) * cosf(qDegreesToRadians(m_YawDeg)));
    m_Position += m_TargetPosition;

    qDebug() << QString("OrbitCamera - Radius: %1, Pitch:%2, Yaw:%3").arg(m_Radius).arg(m_PitchDeg).arg(m_YawDeg);
    qDebug() << "OrbitCamera - Pos:" << m_Position << ", Target:" << m_TargetPosition;
    qDebug() << "OrbitCamera - Up:" << m_Up;
    calcViewMatrix();
}
