#include "World/Camera.h"

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