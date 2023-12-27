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
Camera::Camera()
: mPosition(QVector3D(0.0f, 0.0f, 0.0f)),
  mTargetPos(QVector3D(0.0f, 0.0f, 0.0f)),
  mUp(QVector3D(0.0f, 1.0f, 0.0f)),
  mRight(0.0f, 0.0f, 0.0f),
  WORLD_UP(0.0f, 1.0f, 0.0f),
  mYaw(M_PI),
  mPitch(0.0f),
  mFOV(DEF_FOV)
{
}

//------------------------------------------------------------
// Base Camera - Returns view matrix
//------------------------------------------------------------
QMatrix4x4 Camera::getViewMatrix()const
{
    QMatrix4x4 vMatrix;
    vMatrix.lookAt(mPosition, mTargetPos, mUp);
    return vMatrix;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local look vector
//------------------------------------------------------------
const QVector3D& Camera::getLook() const
{
	return mLook;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local right vector
//------------------------------------------------------------
const QVector3D& Camera::getRight() const
{
	return mRight;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local up vector
//------------------------------------------------------------
const QVector3D& Camera::getUp() const
{
	return mUp;
}

//-----------------------------------------------------------------------------
// FPSCamera - Constructor
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera(QVector3D position, float yaw, float pitch)
{
	mPosition = position;
    mYaw = qDegreesToRadians(yaw);
    mPitch = qDegreesToRadians(pitch);
}

//-----------------------------------------------------------------------------
// FPSCamera - Constructor 
// Given camera staring point and starting target point to look at
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera(QVector3D position, QVector3D target)
{
	mPosition = position;
	mTargetPos = target;

	// Calculate the vector that looks at the target from the camera position
    QVector3D lookDir = position - target;

    // Now Calculate the pitch and yaw from the target look vector.  (radians)
    mPitch = -atan2(lookDir.y(), sqrt(lookDir.x() * lookDir.x() + lookDir.z() * lookDir.z()));
    mYaw = atan2(lookDir.x(), lookDir.z()) + M_PI;
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the camera position in world space
//-----------------------------------------------------------------------------
void FPSCamera::setPosition(const QVector3D& position)
{
	mPosition = position;
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the incremental position of the camera in world space
//-----------------------------------------------------------------------------
void FPSCamera::move(const QVector3D& offsetPos)
{
	mPosition += offsetPos;
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the incremental orientation of the camera
//-----------------------------------------------------------------------------
void FPSCamera::rotate(float yaw, float pitch)
{
    mYaw += qDegreesToRadians(yaw);
    mPitch += qDegreesToRadians(pitch);


	// Constrain the pitch
    mPitch = qBound(-M_PI / 2.0f + 0.1f, mPitch, M_PI / 2.0f - 0.1f);

	// Constraint the yaw [0, 2*pi]
    if (mYaw > (M_PI * 2.0))
        mYaw -= (M_PI * 2.0);
	else if (mYaw < 0.0)
        mYaw += (M_PI * 2.0);


	//std::cout << glm::degrees(mPitch) << " " << glm::degrees(mYaw) << std::endl;

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
    look.setX(cosf(mPitch) * sinf(mYaw));
    look.setY(sinf(mPitch));
    look.setZ(cosf(mPitch) * cosf(mYaw));

    mLook = look.normalized();

	// Re-calculate the Right and Up vector.  For simplicity the Right vector will
    // be assumed horizontal w.r.t. the world's Up vector.
    mRight = QVector3D::crossProduct(mLook, WORLD_UP).normalized();
    mUp = QVector3D::crossProduct(mRight, mLook).normalized();

	mTargetPos = mPosition + mLook;
}

//------------------------------------------------------------
// OrbitCamera - constructor
//------------------------------------------------------------
OrbitCamera::OrbitCamera()
	: mRadius(10.0f)
{}

//------------------------------------------------------------
// OrbitCamera - Sets the target to look at
//------------------------------------------------------------
void OrbitCamera::setLookAt(const QVector3D& target)
{
	mTargetPos = target;
}

//------------------------------------------------------------
// OrbitCamera - Sets the radius of camera to target distance
//------------------------------------------------------------
void OrbitCamera::setRadius(float radius)
{
	// Clamp the radius
    mRadius = qBound(radius, 2.0f, 80.0f);
}

//------------------------------------------------------------
// OrbitCamera - Rotates the camera around the target look
// at position given yaw and pitch in degrees.
//------------------------------------------------------------
void OrbitCamera::rotate(float yaw, float pitch)
{
    mYaw = qDegreesToRadians(yaw);
    mPitch = qDegreesToRadians(pitch);

    mPitch = qBound(mPitch, -M_PI / 2.0f + 0.1f, M_PI / 2.0f - 0.1f);

	// Update Front, Right and Up Vectors using the updated Euler angles
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
    mPosition.setX(mTargetPos.x() + mRadius * cosf(mPitch) * sinf(mYaw));
    mPosition.setY(mTargetPos.y() + mRadius * sinf(mPitch));
    mPosition.setZ(mTargetPos.z() + mRadius * cosf(mPitch) * cosf(mYaw));
}
