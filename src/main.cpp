// namespace
// {
// 	class MyCallback : public RE::BSScript::IStackCallbackFunctor
// 	{
// 	public:
// 		MyCallback() {}
// 		void operator()(RE::BSScript::Variable a_result)
// 		{
// 			// DO SOMETHING!
// 		}
// 		bool CanSave() {
// 			return false;
// 		}
// 		void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) {

// 		}
// 	};


// 	// RE::TESForm::GetAllFormsByEditorID()
// 	// RE::TESForm::LookupFormByEditorID()

// 	// scriptName MyFirstSkse
// 	// 
// 	// string function HelloSkse() global native
// 	//
// 	// string function GetScriptDescription(Quest q)
// 	//

// 	std::vector<std::string> HelloSkse(RE::StaticFunctionTag*, std::string editor_id)
// 	{
// 		auto* form = RE::TESForm::LookupByEditorID(editor_id);
// 		if (!form) {
// 			return {};
// 		}

// 		auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
// 		auto* policy = vm->GetObjectHandlePolicy();

// 		auto handle = policy->GetHandleForObject(form->GetFormType(), form);

// 		auto className = RE::BSFixedString("HelloMod");
// 		auto functionName = RE::BSFixedString("RunMe");
// 		auto args = RE::FunctionArguments<std::string>("I am c++");
// 		auto callback = new MyCallback();
// 		auto callbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>(callback);

// 		vm->DispatchMethodCall(handle, className, functionName, &args, callbackPtr);

// 		auto scriptsMap = vm->attachedScripts.find(handle);

// 		if (scriptsMap == vm->attachedScripts.end()) {
// 			return {};
// 		}

// 		auto& scripts = scriptsMap->second;

// 		if (scripts.size() > 0) {
// 			std::vector<std::string> scriptNames;
// 			scriptNames.reserve(scripts.size());
// 			for (auto& script : scripts) {
// 				auto* typeInfo = script->GetTypeInfo();
// 				scriptNames.emplace_back(typeInfo->GetName());
// 				auto* properties = typeInfo->GetPropertyIter();
// 				for (std::size_t i = 0; i < typeInfo->propertyCount; i++) {
// 					scriptNames.emplace_back(properties[i].name);
// 				}
// 			}
// 			return scriptNames;
// 		} else {
// 			return {};
// 		}
// 	}

// 	bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
// 	{
// 		a_vm->RegisterFunction("HelloSkse", "MyFirstSkse", HelloSkse);
// 		return true;
// 	}

// #ifdef SKSE_1_6
// 	extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
// 		SKSE::PluginVersionData v;

// 		v.PluginVersion(Plugin::VERSION);
// 		v.PluginName(Plugin::NAME);

// 		v.UsesAddressLibrary(true);
// 		v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

// 		return v;
// 	}();
// #endif

// 	extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
// 	{
// 		SKSE::Init(a_skse);

// 		auto papyrus = SKSE::GetPapyrusInterface();
// 		papyrus->Register(RegisterFuncs);

// 		return true;
// 	}

// 	extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* a_info)
// 	{
// 		a_info->infoVersion = SKSE::PluginInfo::kVersion;
// 		a_info->name = Plugin::NAME.data();
// 		a_info->version = 1;  // Plugin::VERSION.pack();

// 		//if (a_skse->IsEditor()) {
// 		//	return false;
// 		//}

// 		//const auto ver = a_skse->RuntimeVersion();
// 		//if (ver < SKSE::RUNTIME_1_5_39) {
// 		//	return false;
// 		//}

// 		return true;
// 	}

// }














#include "Events.h"

namespace {
	void InitializeLog() {
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
		const auto level = spdlog::level::info;

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}

	void Setup() {
		const auto messaging = SKSE::GetMessagingInterface();
		messaging->RegisterListener(Events::OnInit);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);

	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

// Called in all editions
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) {
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	Setup();

	return true;
}

// For Skyrim Special Edition and SkyrimVR
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info) {
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = 1;  // Plugin::VERSION.pack();

	// a_info->name = Plugin::NAME.data();
	// a_info->infoVersion = SKSE::PluginInfo::kVersion;
	// // a_info->infoVersion = Plugin::VERSION.pack();
	// a_info->version = Plugin::VERSION.pack();

	return true;
}
