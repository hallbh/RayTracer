#ifndef _CAMERA_H
#define _CAMERA_H

#include "libs/Matrix.h"

class Camera
{
private:
    Vec3 position;
    // Vector3 lookDir;
    // Vector3 upDir;

    Vec3 u; //
    Vec3 v;
    Vec3 w;

    float fov;

public:
    Camera(Vec3 position, Vec3 direction, Vec3 up, float fov);
	Camera();


    static Camera lookAt(Vec3 position, Vec3 focus, Vec3 up, float fov);

    Vec3 getu() const;
    Vec3 getv() const;
    Vec3 getw() const;
    Vec3 getPosition() const;
    float getFOV() const;
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void moveForward();
	void moveBackward();
	void turnLeft();
	void turnRight();
	void turnUp();
	void turnDown();
	void rollLeft();
	void rollRight();
};

Camera::Camera(Vec3 position, Vec3 direction, Vec3 up, float fov)
    : position(position), fov(fov)//, lookDir(direction), upDir(up)
{
    this->w = normalize(-direction);
    this->u = normalize(Mat::cross(up, this->w));
    this->v = normalize(Mat::cross(this->w, this->u));
}

Camera::Camera()
{
	this->w = normalize(-Vec3{0,0,-1});
    this->u = normalize(Mat::cross(Vec3{0,1,0}, this->w));
    this->v = normalize(Mat::cross(this->w, this->u));
}

Camera Camera::lookAt(Vec3 position, Vec3 focus, Vec3 up, float fov)
{
    return Camera(position, focus - position, up, fov);
}

Vec3 Camera::getu() const
{
    return this->u;
}

Vec3 Camera::getv() const
{
    return this->v;
}

Vec3 Camera::getw() const
{
    return this->w;
}

Vec3 Camera::getPosition() const
{
    return this->position;
}

float Camera::getFOV() const
{
    return this->fov;
}

void Camera::moveLeft() {
	this->position = this->position - this->u * 5.0;
}

void Camera::moveRight() {
	this->position = this->position + this->u * 5.0;
}

void Camera::moveUp() {
	this->position = this->position + this->v * 5.0;
}

void Camera::moveDown() {
	this->position = this->position - this->v * 5.0;
}

void Camera::moveForward() {
	this->position = this->position - this->w * 5.0;
}

void Camera::moveBackward() {
	this->position = this->position + this->w * 5.0;
}

void Camera::turnUp() {
	this->w = normalize(this->w - this->v * 0.05);
	this->v = normalize(Mat::cross(this->w, this->u));
}

void Camera::turnDown() {
	this->w = normalize(this->w + this->v * 0.05);
	this->v = normalize(Mat::cross(this->w, this->u));
}

void Camera::turnLeft() {
	this->w = normalize(this->w + this->u * 0.05);
	this->u = normalize(Mat::cross(this->v, this->w));
}

void Camera::turnRight() {
	this->w = normalize(this->w - this->u * 0.05);
	this->u = normalize(Mat::cross(this->v, this->w));
}

void Camera::rollLeft() {
	this->u = normalize(this->u + this->v * 0.05);
	this->v = normalize(Mat::cross(this->w, this->u));
}

void Camera::rollRight() {
	this->u = normalize(this->u - this->v * 0.05);
	this->v = normalize(Mat::cross(this->w, this->u));
}

#endif
