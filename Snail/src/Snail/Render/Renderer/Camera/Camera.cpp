#include "SNLpch.h"

#include "Camera.h"

namespace Snail {

	Camera::Camera(const float& fov, const float& aspect)
		: m_FOV(fov), m_Aspect(aspect)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
	}

	Camera::Camera(const float& fov, const float& aspect, const float& cnear, const float& cfar)
		: m_FOV(fov), m_Aspect(aspect), m_Near(cnear), m_Far(cfar)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
	}

}