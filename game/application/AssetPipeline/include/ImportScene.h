#pragma once

#include <Package.h>

BEGINNAMESPACE
namespace Importer {
	bool sceneLoad(const path& path, Package& package, uint32 timeStamp);
}
ENDNAMESPACE