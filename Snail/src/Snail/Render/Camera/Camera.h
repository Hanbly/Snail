#pragma once

#include "Snail/Core.h"

#include "glm/glm.hpp"

namespace Snail {

	class Camera {
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Direction;

		glm::mat4 m_ModelMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		// 3. 计算变换 (现在有了头文件，这些函数就能用了)
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		// 4. 设置 Uniforms 到 Shader
	public:
		inline const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		inline void SetModelMatrix(const glm::mat4& mat4) {

		}
	};

}