#pragma once

#include "Snail/Core/Core.h"

namespace Snail {

	enum class VertexDataType {
		None = 0,
		Float, Float2, Float3, Float4, 
		Int, Int2, Int3, Int4, 
		Mat2, Mat3, Mat4,
		Bool
	};

	struct SNAIL_API VertexElement {
		std::string name;
		VertexDataType type;
		uint32_t size;
		uint32_t offset;
		bool enableNormalize;
		VertexElement(std::string name, VertexDataType type, bool enableNormalize = false)
			: name(name), type(type), enableNormalize(enableNormalize){
			size = GetTypeSize();
			offset = size;
		}
		inline uint32_t GetTypeSize() const {
			switch (type) {
				case VertexDataType::None: SNL_CORE_ASSERT(false, "struct VertexElement: 不支持的类型None!");
				case VertexDataType::Float:		return 4 * 1;
				case VertexDataType::Float2:	return 4 * 2;
				case VertexDataType::Float3:	return 4 * 3;
				case VertexDataType::Float4:	return 4 * 4;
				case VertexDataType::Int:		return 4 * 1;
				case VertexDataType::Int2:		return 4 * 2;
				case VertexDataType::Int3:		return 4 * 3;
				case VertexDataType::Int4:		return 4 * 4;
				case VertexDataType::Mat2:		return 4 * 2 * 2;
				case VertexDataType::Mat3:		return 4 * 3 * 3;
				case VertexDataType::Mat4:		return 4 * 4 * 4;
				case VertexDataType::Bool:		return 1;
				default: SNL_CORE_ASSERT(false, "struct VertexElement: 未知顶点元素的类型!");
			}
			return 0;
		}
		inline uint8_t GetComponentCount() const {
			switch (type) {
				case VertexDataType::None: SNL_CORE_ASSERT(false, "struct VertexElement: 不支持的类型None!");
				case VertexDataType::Float:		return 1;
				case VertexDataType::Float2:	return 2;
				case VertexDataType::Float3:	return 3;
				case VertexDataType::Float4:	return 4;
				case VertexDataType::Int:		return 1;
				case VertexDataType::Int2:		return 2;
				case VertexDataType::Int3:		return 3;
				case VertexDataType::Int4:		return 4;
				case VertexDataType::Mat2:		return 2 * 2;
				case VertexDataType::Mat3:		return 3 * 3;
				case VertexDataType::Mat4:		return 4 * 4;
				case VertexDataType::Bool:		return 1;
				default: SNL_CORE_ASSERT(false, "struct VertexElement: 未知顶点元素的类型!");
			}
			return 0;
		}
	};

	class SNAIL_API BufferLayout {
	public:
		virtual ~BufferLayout() {}

		virtual std::vector<VertexElement>& GetLayoutElements() = 0;
		virtual uint32_t GetLayoutSize() const = 0;

		static std::shared_ptr<BufferLayout> CreateBufferLayout(const std::initializer_list<VertexElement>& elements);
	};

}