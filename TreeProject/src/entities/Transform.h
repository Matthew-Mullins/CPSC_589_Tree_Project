#pragma once

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/quaternion.hpp"

class Transform {
public:
	static const glm::vec3 FORWARD;
	static const glm::vec3 RIGHT;
	static const glm::vec3 UP;

	Transform();
	Transform(glm::vec3 _position, glm::vec3 _scale = glm::vec3(1.0f), glm::quat _rotation = glm::quat());

	void Update();

	void SetPosition(glm::vec3 _position);
	void SetScale(glm::vec3 _scale);
	void SetRotation(glm::quat _rotation);
	void SetRotationEulerAngles(glm::vec3 _rotation);
	void SetRotationAxisAngles(glm::vec3 _axis, float _radians);
	void LookAt(glm::vec3 _target);
	void LookInDirection(glm::vec3 _direction);

	void Translate(glm::vec3 _translation);
	void Scale(float _scale);
	void Scale(glm::vec3 _scale);
	void Rotate(glm::vec3 _axis, float _radians);
	void Rotate(glm::quat _quaternion);

	glm::vec3 GetPosition();
	glm::vec3 GetScale();
	glm::quat GetRotation();
	glm::vec3 GetDirection();
	glm::vec3 GetForward();
	glm::vec3 GetRight();
	glm::vec3 GetUp();
	glm::vec3 GetEulerAngles();

	glm::mat4 GetTranslationMatrix();
	glm::mat4 GetScalingMatrix();
	glm::mat4 GetRotationMatrix();
	glm::mat4 GetTransformationMatrix();

private:
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;

	glm::mat4 translationMatrix;
	glm::mat4 scalingMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 transformationMatrix;

	void UpdateTransformationMatrix();
};