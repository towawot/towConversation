#include <skse.h>
#include <skse/PluginAPI.h>
#include <skse/PapyrusVM.h>
#include <skse/GameEvents.h>
#include <skse/GameData.h>
#include <shlobj.h>
#include "CameraHook.h"
#include "Papyrus.h"

#ifdef _DEBUG
	IDebugLog gLog;
#endif

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

// OnMagicEffectApplyを拾うイベントハンドラ
DECLARE_EVENT_HANDLER(MagicEffectApply, MagicEffectApplyEventHandler);
EventResult MagicEffectApplyEventHandler::ReceiveEvent(TESMagicEffectApplyEvent *evn, DispatcherT *dispatcher)
{
	Unregister();
	DataHandler* dhnd = DataHandler::GetSingleton();
	if (dhnd->LookupModByName("towConversation.esp") != NULL)
	{
		#ifdef _DEBUG
			_MESSAGE("hooking player camera.");
		#endif
		CameraHook::Init();

		#ifdef _DEBUG
			_MESSAGE("register papyrus function");
		#endif
		Papyrus::RegisterFuncs((*g_skyrimVM)->GetClassRegistry()); 
	}
	else
	{
		#ifdef _DEBUG
			_MESSAGE("warning: %s is not active.", "towConversation.esp");
		#endif
	}
	return kEvent_Continue;
}

extern "C"
{

bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	#ifdef _DEBUG
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\skse_towConversation.log");
		_MESSAGE("towConversation.dll v%s", "3.1");
	#endif
	// populate info structure
	info->infoVersion =	PluginInfo::kInfoVersion;
	info->name =		"F2F DLL";
	info->version =		1.0;


	// store plugin handle so we can identify ourselves later
	g_pluginHandle = skse->GetPluginHandle();
	
	if(skse->isEditor)
	{
		#ifdef _DEBUG
			_MESSAGE("loaded in editor, marking as incompatible");
		#endif
		return false;
	}

	int major = (skse->runtimeVersion >> 24) & 0xFF;
	int minor = (skse->runtimeVersion >> 16) & 0xFF;
	int build = (skse->runtimeVersion >> 8) & 0xFF;
	int sub   = skse->runtimeVersion & 0xFF;

	if(skse->runtimeVersion != SKSE_SUPPORTING_RUNTIME_VERSION)
	{
		#ifdef _DEBUG
			_MESSAGE("unsupported runtime version 1.%d.%d.%d", major, minor, build);
		#endif

		major = (SKSE_SUPPORTING_RUNTIME_VERSION >> 24) & 0xFF;
		minor = (SKSE_SUPPORTING_RUNTIME_VERSION >> 16) & 0xFF;
		build = (SKSE_SUPPORTING_RUNTIME_VERSION >> 8) & 0xFF;
		sub   = SKSE_SUPPORTING_RUNTIME_VERSION & 0xFF;

		#ifdef _DEBUG
			_MESSAGE("(this plugin needs 1.%d.%d.%d)", major, minor, build);
		#endif
		return false;
	}

	#ifdef _DEBUG
		_MESSAGE("skyrim runtime version: 1.%d.%d.%d.%d ... OK !", major, minor, build);
	#endif

	return true;
}


bool SKSEPlugin_Load(const SKSEInterface * skse)
{
	#ifdef _DEBUG
		_MESSAGE("loaded (handle:%08X)", g_pluginHandle);
	#endif
	static MagicEffectApplyEventHandler s_handlerMagicEffect;
	s_handlerMagicEffect.Register();

	return true;
}

}
