#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include "Variant.h"

/* Usage Information:
======================
- To subscribe a function to an event		-> SUBSCRIBE_TO_EVENT(EVENT_ID, Handler);
- To unsubscribe a function from an event	-> UNSUBSCRIBE_FROM_EVENT(EVENT_ID, Handler);
- To fire off an event						-> FIRE_EVENT(EVENT_ID);
- To fire an event with data				-> FIRE_EVENT_DATA(EVENT_ID, VARIANT);

Note: This is currently a blocking event system, meaning all events are blocked until the current event is handled fully. Alternatively, we can create an event queue and dispatch them 
accordingly instead of executing them instantly at the moment. Not a high priority.
======================
*/

namespace Amethyst
{
	enum class EventType
	{
		FrameEnd,					//A frame ends.
		WindowData,					//The window has a message for processing.
		WorldSave,					//The world must be saved to file.
		WorldSaved,					//The world finished saving to file.
		WorldLoad,					//The world must be loaded from file.
		WorldLoaded,				//The world finished loading from file.
		WorldClear,					//The world should clear everything.
		WorldResolve,				//The world should resolve.
		WorldResolved,				//The world has finished resolving.
		FrameResolutionChanged
	};

	//Macros
	#define EVENT_HANDLER_EXPRESSION(expression)			[this](const Amethyst::Variant& variant) { ##expression }
	#define EVENT_HANDLER_EXPRESSION_STATIC(expression)		[](const Amethyst::Variant& variant) { ##expression }
	
	#define EVENT_HANDLER(function)							[this](const Amethyst::Variant& variant) { function(); }
	#define EVENT_HANDLER_STATIC(function)					[](const Amethyst::Variant& variant) { function(); }
	
	#define EVENT_HANDLER_VARIANT(function)					[this](const Amethyst::Variant& variant) { function(variant); }
	#define EVENT_HANDLER_VARIANT_STATIC(function)			[](const Amethyst::Variant& variant) { function(variant); }
	
	#define	FIRE_EVENT(eventID)								Amethyst::EventSystem::RetrieveInstance().Fire(eventID)
	#define FIRE_EVENT_DATA(eventID, data)					Amethyst::EventSystem::RetrieveInstance().Fire(eventID, data)
	
	#define SUBSCRIBE_TO_EVENT(eventID, function)			Amethyst::EventSystem::RetrieveInstance().Subscribe(eventID, function)
	#define UNSUBSCRIBE_TOEVENT(eventID, function)			Amethyst::EventSystem::RetrieveInstance().Unsubscribe(eventID, function)

	using Subscriber = std::function<void(const Variant&)>;

	class EventSystem
	{
	public:
		static EventSystem& RetrieveInstance()
		{
			static EventSystem instance;
			return instance;
		}

		void Subscribe(const EventType& eventID, Subscriber&& subscriberFunction)
		{
			m_Subscribers[eventID].push_back(std::forward<Subscriber>(subscriberFunction));
		}

		void Unsubscribe(const EventType& eventID, Subscriber&& subscriberFunction)
		{
			const size_t functionAddress = *reinterpret_cast<long*>(reinterpret_cast<char*>(&subscriberFunction));
			std::vector<Subscriber>& subscribers = m_Subscribers[eventID];

			for (auto it = subscribers.begin(); it != subscribers.end();)
			{
				const size_t subscriberAddress = *reinterpret_cast<long*>(reinterpret_cast<char*>(&(*it)));

				if (subscriberAddress == functionAddress)
				{
					it = subscribers.erase(it);
					return;
				}
			}
		}

		void Fire(const EventType eventID, const Variant& data = 0)
		{
			if (m_Subscribers.find(eventID) == m_Subscribers.end()) //If it equals to the end, we can't find it...
			{
				return;
			}

			for (const Amethyst::Subscriber& subscriber : m_Subscribers[eventID]) //For each subscriber function in the events list, we fire said function.
			{
				Subscriber(data); //Bang!
			}
		}

		void ClearAllSubscribers()
		{
			m_Subscribers.clear();
		}

	private:
		std::unordered_map<EventType, std::vector<Subscriber>> m_Subscribers; //Map of an Event Type and a Subscriber Function it has.
	};
}