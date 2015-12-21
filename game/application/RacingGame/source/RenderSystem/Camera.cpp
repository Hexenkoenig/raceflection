#include "RenderSystem/Camera.h"

#include "ObjectSystem/GameObject.h"

//use ACGls camera
#include <ACGL/Scene/GenericCamera.hh>
BEGINNAMESPACE
namespace cam = ACGL::Scene;
Camera::Camera(){
    new(m_CameraStorage) cam::GenericCamera();
}

glm::mat4x4 Camera::getViewMatrix() const {
    const cam::GenericCamera* cb = (const cam::GenericCamera*)m_CameraStorage;
    return cb->getViewMatrix();
}

glm::mat4x4 Camera::getProjectionMatrix() const {
    const cam::GenericCamera* cb = (const cam::GenericCamera*)m_CameraStorage;
    return cb->getProjectionMatrix();
}

glm::uvec2 Camera::getViewportSize() const {
    const cam::GenericCamera* cb = (const cam::GenericCamera*)m_CameraStorage;
    return cb->getViewportSize();
}

void Camera::update() {
    if(!m_BelongTo) return;

    cam::GenericCamera* genCam = (cam::GenericCamera*) m_CameraStorage;
    genCam->setPosition(m_BelongTo->getPosition());
    genCam->setRotationMatrix(glm::mat3_cast(m_BelongTo->getRotation()));
}

void Camera::setViewportSize(glm::uvec2 vec){;
    cam::GenericCamera* genCam = (cam::GenericCamera*)m_CameraStorage;
    genCam->resize(vec.x, vec.y);
}

void Camera::setVerticalFieldOfView(float _fovv){
    cam::GenericCamera* genCam = (cam::GenericCamera*)m_CameraStorage;
    genCam->setVerticalFieldOfView(_fovv);
}

ENDNAMESPACE
