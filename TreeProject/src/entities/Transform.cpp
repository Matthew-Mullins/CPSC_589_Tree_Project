#include "Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

const glm::vec3 Transform::FORWARD = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 Transform::RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Transform::UP = glm::vec3(0.0f, 1.0f, 0.0f);

Transform::Transform() : Transform(glm::vec3(), glm::vec3(1.0f), glm::quat()) {
	UpdateTransformationMatrix();
}

Transform::Transform(glm::vec3 _position, glm::vec3 _scale, glm::quat _rotation) {
	SetPosition(_position);
	SetScale(_scale);
	SetRotation(_rotation);
}

void Transform::Update() {
	SetPosition(position);
	SetScale(scale);
	SetRotation(rotation);
}

void Transform::SetPosition(glm::vec3 _position){
	position = _position;
	translationMatrix = glm::translate(glm::mat4(1.0f), position);
	UpdateTransformationMatrix();
}

void Transform::SetScale(glm::vec3 _scale){
	scale = _scale;
	scalingMatrix = glm::scale(scale);
	UpdateTransformationMatrix();
}

void Transform::SetRotation(glm::quat _rotation){
	rotation = _rotation;
	rotationMatrix = glm::toMat4(rotation);
	UpdateTransformationMatrix();
}

void Transform::SetRotationEulerAngles(glm::vec3 _rotation){
	SetRotation(glm::quat(_rotation));
}

void Transform::SetRotationAxisAngles(glm::vec3 _axis, float _radians){
	SetRotation(glm::angleAxis(_radians, _axis));
}

void Transform::LookAt(glm::vec3 _target){
	LookInDirection(_target - GetPosition());
}

void Transform::LookInDirection(glm::vec3 _direction){
	glm::vec3 direction = glm::normalize(_direction);
	glm::vec3 rotationAxis = cross(FORWARD, direction);
	float dot = glm::dot(FORWARD, direction);
	glm::quat q = glm::quat(dot + 1.0f, rotationAxis);
	SetRotation(glm::normalize(q));
}

void Transform::Translate(glm::vec3 _translation){
	SetPosition(position + _translation);
}

void Transform::Scale(float _scale){
	SetScale(scale * _scale);
}

void Transform::Scale(glm::vec3 _scale){
	SetScale(scale * _scale);
}

void Transform::Rotate(glm::vec3 _axis, float _radians){
	SetRotation(glm::rotate(rotation, _radians, _axis));
}

void Transform::Rotate(glm::quat _quaternion){
	SetRotation(rotation * _quaternion);
}

glm::vec3 Transform::GetPosition(){
	return position;
}

glm::vec3 Transform::GetScale(){
	return scale;
}

glm::quat Transform::GetRotation(){
	return rotation;
}

glm::vec3 Transform::GetDirection(){
	return GetForward();
}

glm::vec3 Transform::GetForward(){
	return rotation * FORWARD;
}

glm::vec3 Transform::GetRight(){
	return rotation * RIGHT;
}

glm::vec3 Transform::GetUp(){
	return rotation * UP;
}

glm::vec3 Transform::GetEulerAngles(){
	return glm::eulerAngles(rotation);
}

glm::mat4 Transform::GetTranslationMatrix(){
	return translationMatrix;
}

glm::mat4 Transform::GetScalingMatrix(){
	return scalingMatrix;
}

glm::mat4 Transform::GetRotationMatrix(){
	return rotationMatrix;
}

glm::mat4 Transform::GetTransformationMatrix(){
	return transformationMatrix;
}

void Transform::UpdateTransformationMatrix(){
	transformationMatrix = translationMatrix * rotationMatrix * scalingMatrix;
}

