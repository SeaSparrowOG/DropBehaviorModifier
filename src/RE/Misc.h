#pragma once

#include "offset.h"

namespace RE
{
	inline void DebugNotification(const char* a_notification, const char* a_soundToPlay, bool a_cancelIfAlreadyQueued)
	{
		using func_t = decltype(&DebugNotification);
		static REL::Relocation<func_t> func{ Offset::DebugNotification };
		return func(a_notification, a_soundToPlay, a_cancelIfAlreadyQueued);
	}

	void CreateMessage(const char* a_message, IMessageBoxCallback* a_callback, std::uint32_t a_arg3, std::uint32_t a_arg4, std::uint32_t a_arg5, const char* a_buttonText, const char* a_secondaryButtonText)
	{
		using func_t = decltype(&CreateMessage);
		static REL::Relocation<func_t> func{ RE::Offset::CreateMessage };
		return func(a_message, a_callback, a_arg3, a_arg4, a_arg5, a_buttonText, a_secondaryButtonText);
	}

	void GetMessageText(RE::TESDescription* a_this, const char* a_result, void* a_3 = nullptr, std::uint32_t a_4 = 0x43534544)
	{
		using func_t = decltype(&GetMessageText);
		static REL::Relocation<func_t> func{ RE::Offset::Message::GetDescription };
		return func(a_this, a_result, a_3, a_4);
	}

	void DebugMessageBox(const char* a_message)
	{
		CreateMessage(a_message, nullptr, 0, 4, 10, GameSettingCollection::GetSingleton()->GetSetting("sOk")->GetString(), nullptr);
	}
}