#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Hyperion
{
	class Camera
	{
	public:
		enum class ProjectionType { Perspective, Orthographic, MAX };

	public:
		const glm::mat4& getProjection() const { return m_projection; }
		const glm::mat4& getView() const { return m_view; }
		const glm::mat4& getInvView() const { return m_invView; }

		ProjectionType getProjectionType() const { return m_projectionType; }
		void setProjectionType(ProjectionType projectionType);

		// width and height can be either pixels or screen coords (only used for aspect ratio)
		void setViewportSize(uint32_t width, uint32_t height);
		void setPerspective(float fovy, float nearClip, float farClip);
		void setOrthographic(float size, float nearClip, float farClip);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up);
		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

	private:
		void recalculateProjection();

		void recalculateOrthographicProjection(
			float left, float right, float top, float bottom, float near, float far);
		void recalculatePerspectiveProjection(float fovy, float aspect, float near, float far);

	private:
		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_invView = glm::mat4(1.0f);
		ProjectionType m_projectionType = ProjectionType::Perspective;

		float m_perspectiveFOVy = glm::radians(45.0f);
		float m_perspectiveNear = 0.01f;
		float m_perspectiveFar = 1000.0f;

		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0;
		float m_orthographicFar = 1.0f;

		float m_aspectRatio = 0.0f;
	};
}