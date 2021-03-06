#pragma once

#include <Utilities/UUID.h>

BEGINNAMESPACE

struct TextureSpec {
	
	UUID uuid;
	
	static const uint32 MaxTextureNameLength = 80;
	ansichar m_TextureName[MaxTextureNameLength]; //contains the "stem" of the texture file
	uint32 m_DataSize;

	union {
		Byte* m_TextureData; //A dds file in memory
		UIntOfPtrSize __TextureDataLocation;
	};
	static const TextureSpec* FromBuffer(const Byte* buffer);
	static uint32 MemSize(const TextureSpec* spec);
	static void ToBuffer(const TextureSpec* spec, Byte* buffer);
};

ENDNAMESPACE
