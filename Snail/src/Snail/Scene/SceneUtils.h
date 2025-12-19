#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Scene/Entity.h"
#include "Snail/Scene/Component.h"

#include "Snail/Render/Renderer/Material/ShaderLibrary.h"
#include "Snail/Render/Renderer/Material/TextureLibrary.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "boost/uuid/uuid_io.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/uuid/string_generator.hpp"

#include "yaml-cpp/yaml.h"

namespace Snail {
    // ------------------------------------ 鼠标射线 -----------------------------------------------------
    struct MouseRay {
        glm::vec3 origin;    // 射线起点
        glm::vec3 direction; // 射线方向（单位向量）

        MouseRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& view, const glm::mat4& proj);

        bool Is_Cross(Entity& entity, float& outDistance) const;
    };

    // ------------------------------------ 序列化 & 反序列化 -------------------------------------------------------
    class SceneSerializer
    {
    public:
        SceneSerializer(const Refptr<Scene>& scene, const Refptr<EditorCamera>& ec = nullptr);

		void Serialize(const std::string& sceneName, const std::string& filepath);
        // void SerializeRuntime(const std::string& filepath); // 未来实现

        bool Deserialize(const std::string& filepath);
        // bool DeserializeRuntime(const std::string& filepath); // 未来实现

    private:
        // 序列化单个 Entity
        void SerializeEntity(YAML::Emitter& out, Entity entity);
        // 反序列化单个 Entity
        void DeserializeEntity(const YAML::Node& entityNode, Entity entity);

    private:
		Refptr<Scene> m_Scene;
		Refptr<EditorCamera> m_EC = nullptr;
    };

} // namespace Snail

namespace YAML {

	// UUID 特化
	template<>
	struct convert<boost::uuids::uuid> {
		static Node encode(const boost::uuids::uuid& rhs) {
			return Node(boost::uuids::to_string(rhs));
		}
		static bool decode(const Node& node, boost::uuids::uuid& rhs) {
			if (!node.IsScalar()) return false;
			// 需要包含 <boost/uuid/string_generator.hpp>
			boost::uuids::string_generator gen;
			rhs = gen(node.as<std::string>());
			return true;
		}
	};

	// glm::vec2 特化
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	// glm::vec3 特化
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow); // [x, y, z] 格式
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	// glm::vec4 特化
	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow); // [x, y, z] 格式
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	// glm::mat4 特化
	template<>
	struct convert<glm::mat4> {
		static Node encode(const glm::mat4& rhs) {
			Node node;
			const float* pSource = (const float*)glm::value_ptr(rhs);
			for (int i = 0; i < 16; ++i) node.push_back(pSource[i]);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::mat4& rhs) {
			if (!node.IsSequence() || node.size() != 16) return false;
			float* pDest = (float*)glm::value_ptr(rhs);
			for (int i = 0; i < 16; ++i) pDest[i] = node[i].as<float>();
			return true;
		}
	};

	template<>
	struct convert<std::vector<std::string>> {
		static Node encode(const std::vector<std::string>& rhs) {
			Node node;
			for (const auto& s : rhs) node.push_back(s);
			node.SetStyle(EmitterStyle::Flow); // 使用 [path1, path2] 格式
			return node;
		}
		static bool decode(const Node& node, std::vector<std::string>& rhs) {
			if (!node.IsSequence()) return false;
			for (auto it = node.begin(); it != node.end(); ++it) {
				rhs.push_back(it->as<std::string>());
			}
			return true;
		}
	};

	static Emitter& operator<<(Emitter& out, const boost::uuids::uuid& v) {
		out << Node(v);
		return out;
	}

	static Emitter& operator<<(Emitter& out, const glm::vec2& v) {
		out << Node(v);
		return out;
	}

	static Emitter& operator<<(Emitter& out, const glm::vec3& v) {
		out << Node(v);
		return out;
	}

	static Emitter& operator<<(Emitter& out, const glm::vec4& v) {
		out << Node(v);
		return out;
	}

	static Emitter& operator<<(Emitter& out, const glm::mat4& v) {
		out << Node(v);
		return out;
	}

	static Emitter& operator<<(Emitter& out, const std::vector<std::string>& v) {
		out << Node(v);
		return out;
	}

	static Emitter& operator<<(Emitter& out, const Snail::Vertex& v) {
		out << BeginMap; // 每个顶点是一个 Map
		out << Key << "Position" << Value << v.position;
		out << Key << "Normal" << Value << v.normal;
		out << Key << "TexCoords" << Value << v.texCoords;
		out << YAML::EndMap;
		return out;
	}

}
