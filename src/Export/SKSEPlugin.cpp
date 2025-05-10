#include "Events/Events.h"
#include "Hooks/Hooks.h"
#include "Settings/INISettings.h"
#include "Settings/JSONSettings.h"

namespace
{
	void InitializeLog()
	{
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

#ifndef NDEBUG
		const auto level = spdlog::level::debug;
#else 
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
	{
		SKSE::PluginVersionData v{};

		v.PluginVersion(Plugin::VERSION);
		v.PluginName(Plugin::NAME);
		v.AuthorName("SeaSparrow"sv);
		v.UsesAddressLibrary(true);

		return v;
	}();

extern "C" DLLEXPORT bool SKSEAPI
SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_6_1130) {
		return false;
	}

	return true;
}

static void MessageEventCallback(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		if (!Settings::JSON::Holder::GetSingleton()->Read()) {
			SKSE::stl::report_and_fail("Failed to read JSON settings."sv);
		}
		logger::info("==========================================================");
		if (!Events::Install()) {
			SKSE::stl::report_and_fail("Failed to register event listeners."sv);
		}
		logger::info("==========================================================");
		logger::info("Startup tasks finished, enjoy your game!");
		break;
	default:
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	logger::info("=================================================");
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
	logger::info("Author: SeaSparrow"sv);
	logger::info("=================================================");
	SKSE::Init(a_skse);

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_6_1130) {
		return false;
	}

	logger::info("Performing startup tasks..."sv);
	if (!Settings::INI::Holder::GetSingleton()->Read()) {
		SKSE::stl::report_and_fail("Failed to read INI settings."sv);
	}
	if (!Hooks::Install()) {
		SKSE::stl::report_and_fail("Failed to install necessary hooks."sv);
	}

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(&MessageEventCallback);

	return true;
}