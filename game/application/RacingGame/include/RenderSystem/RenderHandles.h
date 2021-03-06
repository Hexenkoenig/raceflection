#pragma once

#include "Utilities/Handle.h"

BEGINNAMESPACE

typedef Handle<12, 20> VertexBufferHandle;
typedef Handle<12, 20> IndexBufferHandle;
typedef Handle<12, 20> RenderTargetHandle;
typedef Handle<12, 20> VertexLayoutHandle;
typedef Handle<12, 20> ShaderProgramHandle;
typedef Handle<12, 20> ConstantBufferHandle;
typedef Handle<16, 16> MaterialHandle;
typedef Handle<16, 16> TextureHandle;
typedef Handle<12, 20> GeometryHandle;

INVALID_HANDLE_DECL(VertexBufferHandle);
INVALID_HANDLE_DECL(IndexBufferHandle);
INVALID_HANDLE_DECL(RenderTargetHandle);
INVALID_HANDLE_DECL(VertexLayoutHandle);
INVALID_HANDLE_DECL(ShaderProgramHandle);
INVALID_HANDLE_DECL(ConstantBufferHandle);
INVALID_HANDLE_DECL(MaterialHandle);
INVALID_HANDLE_DECL(TextureHandle);
INVALID_HANDLE_DECL(GeometryHandle);

ENDNAMESPACE
