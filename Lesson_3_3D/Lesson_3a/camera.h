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

class Camera
{
public:
    QMatrix4x4 getViewMatrix() const;

    virtual void setPosition(const QVector3D& position) {}
    virtual void rotate(float yaw, float pitch) {}  // in degrees
    virtual void move(const QVector3D& offsetPos) {}

    const QVector3D& getLook() const;
    const QVector3D& getRight() const;
    const QVector3D& getUp() const;

	float getFOV() const   { return mFOV; }
	void setFOV(float fov) { mFOV = fov; }		// in degrees

protected:
	Camera();

	virtual void updateCameraVectors() {}

    QVector3D mPosition;
    QVector3D mTargetPos;
    QVector3D mLook;
    QVector3D mUp;
    QVector3D mRight;
    const QVector3D WORLD_UP;

	// Euler Angles (in radians)
	float mYaw;
	float mPitch;

	// Camera parameters
	float mFOV; // degrees
};

//--------------------------------------------------------------
// FPS Camera Class
//--------------------------------------------------------------
class FPSCamera : public Camera
{
public:

    FPSCamera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), float yaw = M_PI, float pitch = 0.0f); // (yaw) initial angle faces -Z
    FPSCamera(QVector3D position, QVector3D target);
    virtual void setPosition(const QVector3D& position);
	virtual void rotate(float yaw, float pitch);	// in degrees
    virtual void move(const QVector3D& offsetPos);

private:

	void updateCameraVectors();
};


//--------------------------------------------------------------
// Orbit Camera Class
//--------------------------------------------------------------
class OrbitCamera : public Camera
{
public:

	OrbitCamera();

	virtual void rotate(float yaw, float pitch);    // in degrees

	// Camera Controls
    void setLookAt(const QVector3D& target);
	void setRadius(float radius);

private:

	void updateCameraVectors();

	// Camera parameters
	float mRadius;
};
