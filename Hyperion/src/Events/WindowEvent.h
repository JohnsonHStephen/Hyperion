#pragma once

#include "Event.h"

namespace Hyperion
{
	class WindowCloseEvent: public Event {
	public:
		WindowCloseEvent() = default;

		inline EventType GetEventType() const override { return EventType::WindowClose; }
		inline int GetCategories() const override { return EVENT_WINDOW; }

		std::string ToString() const override {
			return "WindowCloseEvent";
		}
	};

	class WindowResizeEvent: public Event {
	public:
		WindowResizeEvent(unsigned int width, unsigned int height):
			m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		inline EventType GetEventType() const override { return EventType::WindowResize; }
		inline int GetCategories() const override { return EVENT_WINDOW; }

		std::string ToString() const override {
			return "WindowCloseEvent";
		}
	private:
		unsigned int m_Width, m_Height;
	};
}