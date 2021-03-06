#include "InputSystem/InputComponent.h"
#include "InputSystem/InputDevice.h"

#include "Configuration/ConfigSettings.h"

#include "ObjectSystem/GameObject.h"

BEGINNAMESPACE

InputComponent::InputComponent(InputDevice device) :
    m_InputDevice(device)
{}

InputComponent::~InputComponent(){}

bool InputComponent::process(float32 dt, GameObject *){ return true; }


///
/// Sample Input Component
///

ConfigSettingFloat32 cfgInputVelocity("InputVelocity", "Sets the input velocity in m/s.", 10.0f);
ConfigSettingFloat32 cfgInputRotationVelocity("InputRotationVelocity", "Sets the input rotaton velocity in rad/s", glm::pi<float32>() / 4.0);
ConfigSettingFloat32 cfgMouseSensitivity("MouseSensivity", "Sets the mouse sensivity", 20.0f);

InputWASDComponent::InputWASDComponent(InputDevice device) :
    InputComponent(device)
{
    m_FwdTrigger = m_InputDevice.addTrigger(
                    &Or<
                        &Key::IsPressed<Keyboard::Code::key_W>,
                        &Key::IsPressed<Keyboard::Code::key_UP>
                    >
                );

    m_BackTrigger = m_InputDevice.addTrigger(
                    &Or<
                        &Key::IsPressed<Keyboard::Code::key_S>,
                        &Key::IsPressed<Keyboard::Code::key_DOWN>
                    >
                );

    m_LeftTrigger = m_InputDevice.addTrigger(
                    &Or<
                        &Key::IsPressed<Keyboard::Code::key_A>,
                        &Key::IsPressed<Keyboard::Code::key_LEFT>
                    >
                );

    m_RightTrigger = m_InputDevice.addTrigger(
                    &Or<
                        &Key::IsPressed<Keyboard::Code::key_D>,
                        &Key::IsPressed<Keyboard::Code::key_RIGHT>
                    >
                );

    m_UpTrigger = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_SPACE>
                );
#	if DEBUG_RELEASE || OS_LINUX
    m_DownTrigger = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_LCONTROL>
                );
#	else
	m_DownTrigger = m_InputDevice.addTrigger(
		&Key::IsPressed<Keyboard::Code::key_RCONTROL>
		);
#	endif

    m_MouseButtonTrigger = m_InputDevice.addTrigger(
                    &MouseButton::IsPressed<Mouse::Button::Right>
                );

    m_TurnLeft = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_Q>
				);

    m_TurnRight = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_E>
				);

    m_TurnClockwise = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_X>
                );

    m_TurnCounterClockwise = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_Y>
                );

    m_TurnDown = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_C>
                );

    m_TurnUp = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_R>
                );

    m_Nitro = m_InputDevice.addTrigger(
                    &Key::IsPressed<Keyboard::Code::key_LSHIFT>
                );

    m_Reset = m_InputDevice.addTrigger(
                    &Or<
                        &Key::IsPressed<Keyboard::Code::key_T>,
                        &Key::IsPressed<Keyboard::Code::key_NUMPAD0>
                    >
                );
}

InputWASDComponent::~InputWASDComponent(){}

bool InputWASDComponent::process(float32 dt, GameObject *object){
    static glm::vec3 lastMousePosition = glm::vec3(-1.0, -1.0, -1.0);
    glm::vec3 pos = object->getPosition();

    if(m_InputDevice.isTriggered(m_Reset)){
        object->lookInDirection(GameObject::FORWARD_DIRECTION, GameObject::UP_DIRECTION);
        return true;
    }

    float32 frac = cfgInputVelocity * 1000.0 / dt;

    if(m_InputDevice.isTriggered((m_Nitro))){
        frac *= 4;
    }
    if(m_InputDevice.isTriggered(m_FwdTrigger)){
        pos += frac * object->getForward();
    }
    if(m_InputDevice.isTriggered(m_BackTrigger)){
        pos += frac * object->getBackward();
    }
    if(m_InputDevice.isTriggered(m_LeftTrigger)){
        pos += frac * object->getLeft();
    }
    if(m_InputDevice.isTriggered(m_RightTrigger)){
        pos += frac * object->getRight();
    }
    if(m_InputDevice.isTriggered(m_UpTrigger)){
        pos += frac * object->getUp();
    }
    if(m_InputDevice.isTriggered(m_DownTrigger)){
        pos += frac * object->getDown();
    }
    object->setPosition(pos);

	float32 rFrac = cfgInputRotationVelocity * 1000.0 / dt;
	glm::quat quat = object->getRotation();
	if (m_InputDevice.isTriggered(m_TurnLeft)) {
		quat = quat * glm::angleAxis(-rFrac, object->getUp());
	}
	if (m_InputDevice.isTriggered(m_TurnRight)) {
		quat = quat * glm::angleAxis(rFrac, object->getUp());
	}
    if (m_InputDevice.isTriggered(m_TurnClockwise)) {
        quat = quat * glm::angleAxis(-rFrac, object->getForward());
    }
    if (m_InputDevice.isTriggered(m_TurnCounterClockwise)) {
        quat = quat * glm::angleAxis(rFrac, object->getForward());
    }
	if (m_InputDevice.isTriggered(m_TurnDown)) {
        quat = quat * glm::angleAxis(-rFrac, object->getLeft());
	}
	if (m_InputDevice.isTriggered(m_TurnUp)) {
        quat = quat * glm::angleAxis(rFrac, object->getLeft());
	}
	object->setRotation(quat);

    Point3f p = m_InputDevice.getMouse()->position();
    if(m_InputDevice.isTriggered(m_MouseButtonTrigger)){
		
        //rotation implementation here
		glm::vec2 dir(lastMousePosition.x - p.x, lastMousePosition.y - p.y);
		if (dir.x > glm::epsilon<float32>() || dir.y > glm::epsilon<float32>())
		{
			dir = glm::normalize(dir);

		
		}
    }

    lastMousePosition = glm::vec3(p.x, p.y, p.z);

	return true;
}


ENDNAMESPACE
