#pragma once

#include <vector>
#include "ObjectSystem/GameObject.h"
#include "MemorySystem.h"

BEGINNAMESPACE

#define MAX_OBJECTS 1000

using GameObjectID = int;

class PackageSpec;

class ObjectSystem {

    typedef ProxyAllocator < PoolAllocator, policy::NoSync, policy::NoBoundsChecking, policy::NoTracking, policy::NoTagging> GameObjectAlloc;

private:

    //std::vector<GameObject> mGameObjects;

    GameObjectAlloc mGameObjects;


public:
    ObjectSystem();
    ~ObjectSystem();

    template<typename... args>
	GameObject* createObject(args... arguments){
            GameObject* object = eng_new(GameObject, mGameObjects)(arguments...);
			object->mID = getElementIndex(object, mGameObjects);
            return object;
    }

    void deleteObject(GameObjectID ID);

    GameObject* getObjectByID(GameObjectID ID);

    bool isTriggerArea(GameObjectID ID);

	bool createObjectsFromPackageSpec(PackageSpec* pkgSpec);

};

ENDNAMESPACE