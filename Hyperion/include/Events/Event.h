#pragma once

#define EVENT_WINDOW	(1 << 0)
#define EVENT_KEY		(1 << 1)
#define EVENT_MOUSE		(1 << 2)

namespace Hyperion
{
	enum class EventType {
		None = 0,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased, CharTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	class Event {
	public:
		~Event() = default;

		inline bool IsHandled() const { return Handled; }
		void SetHandled(bool h = true) { Handled = h; }

		virtual EventType GetEventType() const = 0;
		virtual int GetCategories() const = 0;

		virtual std::string ToString() const = 0;

		inline bool IsInCategory(int c) const { return GetCategories() & c; }

	private:
		bool Handled = false;
	};
}