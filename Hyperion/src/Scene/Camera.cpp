#include "Scene/Camera.h"

namespace Hyperion
{
	void Camera::setProjectionType(ProjectionType projectionType)
	{
		m_projectionType = projectionType;
		recalculateProjection();
	}

	void Camera::setViewportSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			throw std::runtime_error("Cannot set width or hight camera viewsize to 0");

		m_aspectRatio = (float)width / (float)height;
		recalculateProjection();
	}

	void Camera::setPerspective(float fovy, float nearClip, float farClip)
	{
		m_perspectiveFOVy = fovy;
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;

		recalculateProjection();
	}

	void Camera::setOrthographic(float size, float nearClip, float farClip)
	{
		m_orthographicSize = size;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;

		recalculateProjection();
	}

	void Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
	{
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		m_view = glm::mat4{ 1.f };
		m_view[0][0] = u.x;
		m_view[1][0] = u.y;
		m_view[2][0] = u.z;
		m_view[0][1] = v.x;
		m_view[1][1] = v.y;
		m_view[2][1] = v.z;
		m_view[0][2] = w.x;
		m_view[1][2] = w.y;
		m_view[2][2] = w.z;
		m_view[3][0] = -glm::dot(u, position);
		m_view[3][1] = -glm::dot(v, position);
		m_view[3][2] = -glm::dot(w, position);

		m_invView = glm::mat4{ 1.f };
		m_invView[0][0] = u.x;
		m_invView[0][1] = u.y;
		m_invView[0][2] = u.z;
		m_invView[1][0] = v.x;
		m_invView[1][1] = v.y;
		m_invView[1][2] = v.z;
		m_invView[2][0] = w.x;
		m_invView[2][1] = w.y;
		m_invView[2][2] = w.z;
		m_invView[3][0] = position.x;
		m_invView[3][1] = position.y;
		m_invView[3][2] = position.z;
	}

	void Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
	{
		setViewDirection(position, target - position, up);
	}

	void Camera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		m_view = glm::mat4{ 1.f };
		m_view[0][0] = u.x;
		m_view[1][0] = u.y;
		m_view[2][0] = u.z;
		m_view[0][1] = v.x;
		m_view[1][1] = v.y;
		m_view[2][1] = v.z;
		m_view[0][2] = w.x;
		m_view[1][2] = w.y;
		m_view[2][2] = w.z;
		m_view[3][0] = -glm::dot(u, position);
		m_view[3][1] = -glm::dot(v, position);
		m_view[3][2] = -glm::dot(w, position);

		m_invView = glm::mat4{ 1.f };
		m_invView[0][0] = u.x;
		m_invView[0][1] = u.y;
		m_invView[0][2] = u.z;
		m_invView[1][0] = v.x;
		m_invView[1][1] = v.y;
		m_invView[1][2] = v.z;
		m_invView[2][0] = w.x;
		m_invView[2][1] = w.y;
		m_invView[2][2] = w.z;
		m_invView[3][0] = position.x;
		m_invView[3][1] = position.y;
		m_invView[3][2] = position.z;
	}

	void Camera::recalculateProjection()
	{
		switch (m_projectionType)
		{
		case ProjectionType::Perspective:
		{
			recalculatePerspectiveProjection(m_perspectiveFOVy, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
			break;
		}
		case ProjectionType::Orthographic:
		{
			float orthoLeft = -0.5f * m_aspectRatio * m_orthographicSize;
			float orthoRight = 0.5f * m_aspectRatio * m_orthographicSize;
			float orthoTop = 0.5f * m_orthographicSize;
			float orthoBottom = -0.5f * m_orthographicSize;

			recalculateOrthographicProjection(orthoLeft, orthoRight, orthoTop, orthoBottom, m_orthographicNear, m_orthographicFar);
			break;
		}
		default:
			throw std::runtime_error("Unsupported camera projection type");
			break;
		}
	}

	void Camera::recalculateOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
	{
		m_projection = glm::mat4{ 1.0f };
		m_projection[0][0] = 2.f / (right - left);
		m_projection[1][1] = 2.f / (bottom - top);
		m_projection[2][2] = 1.f / (far - near);
		m_projection[3][0] = -(right + left) / (right - left);
		m_projection[3][1] = -(bottom + top) / (bottom - top);
		m_projection[3][2] = -near / (far - near);
	}

	void Camera::recalculatePerspectiveProjection(float fovy, float aspect, float near, float far)
	{
		const float tanHalfFovy = tan(fovy / 2.f);
		m_projection = glm::mat4{ 0.0f };
		m_projection[0][0] = 1.f / (aspect * tanHalfFovy);
		m_projection[1][1] = 1.f / (tanHalfFovy);
		m_projection[2][2] = far / (far - near);
		m_projection[2][3] = 1.f;
		m_projection[3][2] = -(far * near) / (far - near);
	}
}