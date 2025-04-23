#pragma once

#include "Event.h"

namespace Hyperion
{
	enum class MouseCode: uint16_t {
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	};

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode) {
		os << static_cast<int32_t>(mouseCode);
		return os;
	}

	class MouseButtonEvent: public Event {
	public:
		inline MouseCode GetMouseButton() const { return m_Button; }

		inline int GetCategories() const override { return EVENT_MOUSE; }

	protected:
		MouseButtonEvent(MouseCode button):
			m_Button(button) {}

		MouseCode m_Button;
	};

	class MouseButtonPressedEvent: public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(MouseCode button):
			MouseButtonEvent(button) {}

		inline EventType GetEventType() const override { return EventType::MouseButtonPressed; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}
	};

	class MouseButtonReleasedEvent: public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(MouseCode button):
			MouseButtonEvent(button) {}

		inline EventType GetEventType() const override { return EventType::MouseButtonReleased; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}
	};

	class MouseScrolledEvent: public Event {
	public:
		MouseScrolledEvent(float xOffset, float yOffset):
			m_xOffset(xOffset), m_yOffset(yOffset) {}

		inline EventType GetEventType() const override { return EventType::MouseScrolled; }
		inline int GetCategories() const override { return EVENT_MOUSE; }

		inline float GetXOffset() const { return m_xOffset; }
		inline float GetYOffset() const { return m_yOffset; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_xOffset << ", " << m_yOffset;
			return ss.str();
		}

	private:
		float m_xOffset, m_yOffset;
	};

	class MouseMovedEvent: public Event {
	public:
		MouseMovedEvent(float x, float y):
			m_MouseX(x), m_MouseY(y) {}

		inline EventType GetEventType() const override { return EventType::MouseMoved; }
		inline int GetCategories() const override { return EVENT_MOUSE; }

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseX;
			return ss.str();
		}

	private:
		float m_MouseX, m_MouseY;
	};
}