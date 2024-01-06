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

#include <QVector3D>
#include <QVector4D>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>

class ICamera
{
public:

    QMatrix4x4 viewMatrix() const { return m_viewMatrix; }

    virtual void setPosition(const QVector3D& position);
    QVector3D position() const { return m_Position; }

    void setTargetPosition( const QVector3D & targetPosition );
    QVector3D targetPosition() const { return m_TargetPosition; }

    virtual void rotate(float yawDegrees, float pitchDegrees);
    virtual void setRotation(float yawDegrees, float pitchDegrees);
    virtual void move(const QVector3D& offsetPos);
    virtual void setLookAt(const QVector3D& target);

    // Direction of view as unit vectors
    const QVector3D& lookVector() const;
    const QVector3D& rightVector() const;
    const QVector3D& upVector() const;

    float getFOV() const   { return m_FovDegrees; }
    void setFOV(float fovDegrees) { m_FovDegrees = fovDegrees; }

protected:
    ICamera();
    virtual ~ICamera() = default;

    virtual void updateCameraVectors() = 0;

    void setViewMatrix(const QMatrix4x4 & view);
    void calcViewMatrix();

    QMatrix4x4 m_viewMatrix;
    QVector3D m_Position;
    QVector3D m_TargetPosition;
    QVector3D m_Look;
    QVector3D m_Up;
    QVector3D m_Right;
    const QVector3D WORLD_UP;

    // Euler Angles (in degrees)
    float m_YawDeg { 0.0f };
    float m_PitchDeg { 0.0f };

    // Default camera values degrees
    const float DEF_FOV = 45.0f;

	// Camera parameters
    float m_FovDegrees { DEF_FOV };
};

//--------------------------------------------------------------
// Player's bodycam camera Class
//--------------------------------------------------------------
class PlayerCamera : public ICamera
{
public:

    // Default initial angle faces -Z (Yaw 0 degrees, 180 degrees is added internally)
    PlayerCamera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), float yawDegrees = 0.0, float pitch = 0.0f);
    PlayerCamera(QVector3D position, QVector3D target);

protected:

    void updateCameraVectors() override;
};

//--------------------------------------------------------------
// Orbit Camera Class
//--------------------------------------------------------------
class OrbitCamera : public ICamera
{
public:

    OrbitCamera(float radius, float yawDegrees, float pitchDegrees);
    OrbitCamera();

    // Move nearer or further away
    void setRadius(float radius);
    float radius() const {return m_Radius; }

    // Set the center or the orbit (default 0,0,0)
    void setOrbitCenter( const QVector3D & center) { setTargetPosition(center); }

protected:

    void updateCameraVectors() override;

private:
	// Camera parameters
    float m_Radius { 1.0f };
};
