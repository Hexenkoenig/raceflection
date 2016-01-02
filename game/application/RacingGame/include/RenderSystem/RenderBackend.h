#pragma once

#include "RenderSystem/RenderHandles.h"
#include "RenderSystem/RenderTypes.h"

namespace nvFX { //The backend should create valid "backend-objects" for the effect system
	class ICstBuffer;
}

BEGINNAMESPACE

struct VertexLayoutSpec;
struct GeometrySpec;
struct TextureSpec;

class RenderBackend {
public:
	virtual ~RenderBackend(){}
	virtual bool startupBackend() { return true; }
    virtual bool initializeContext() { return true; }
	virtual void shutdownBackend() {}

	/*
	virtual VertexBufferHandle createStaticVertexBuffer(size_type bufferSize, Byte* pInitialData) {	return VertexBufferHandle(); }
	virtual VertexBufferHandle createDynamicVertexBuffer(size_type bufferSize, Byte* pInitialData) { return VertexBufferHandle(); }
	virtual void destroyVertexBuffer(VertexBufferHandle vb) {}

	virtual IndexBufferHandle createIndexBuffer(size_type bufferSize, void* pInitialData) { return IndexBufferHandle(); }
	virtual void destroyIndexBuffer(IndexBufferHandle ib) {}
	*/
	virtual GeometryHandle createGeometry(const GeometrySpec* specification) { return InvalidGeometryHandle; };
	virtual bool updateGeometry(GeometryHandle handle, const GeometrySpec* specification) { return false; }
	virtual VertexLayoutHandle createVertexLayout(const VertexLayoutSpec* specification) { return InvalidVertexLayoutHandle; }

	//
	/// Texture
	virtual TextureHandle createTexture(const TextureSpec* specification) { return InvalidTextureHandle; }
	virtual bool updateTexture(TextureHandle handle, const TextureSpec* specification) { return false;  }

	//
	/// Shader
	virtual ShaderProgramHandle createShaderProgram(ShaderProgramSpec specification) { return InvalidShaderProgramHandle; }

	virtual ConstantBufferHandle createConstantBuffer(nvFX::ICstBuffer* effectBuffer, uint32 size) { return InvalidConstantBufferHandle; }
	virtual ConstantBufferHandle createConstantBuffer(ConstantBufferSpec specification) { return InvalidConstantBufferHandle; }
	virtual void destroyConstantBufferFX(ConstantBufferHandle){}
	virtual void destroyConstantBuffer(ConstantBufferHandle){}

	virtual RenderTargetHandle createRenderTarget(RenderTargetLayout rtl) { return InvalidRenderTargetHandle; }
};

ENDNAMESPACE

#include "RenderSystem/Null/NullBackend.h"
#include "RenderSystem/OpenGL/OpenGLBackend.h"
