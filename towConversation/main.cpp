#include <skse.h>
#include <skse/PluginAPI.h>
#include <skse/GameAPI.h>
// #include <skse/GameForms.h>
#include <skse/GameReferences.h>
#include <skse/GameTESEvents.h>
#include <skse/GamePapyrusFunctions.h>
#include <skse/ScaleformCallbacks.h>
#include <skse/ScaleformMovie.h>
#include <shlobj.h>

#include <skse/GameData.h>
#include <skse/GameRTTI.h>

#include "CameraHook.h"
#include "Papyrus.h"

#ifdef _DEBUG
IDebugLog gLog;
#endif

/*=================================
/ skse plugin
/==================================*/

class towConversation : public SKSEPlugin
{
public:
	towConversation()
	{
		#ifdef _DEBUG
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\skse_towConversation.log");
		#endif
	}

	//====================================================
	// ここでプラグイン・インスタンスの初期化処理をする
	// SKSEPlugin_Query() に相当
	//====================================================
	virtual bool InitInstance()
	{
		// towConversation.esp が MODリストに無けれれば、プラグインをロードしない
		if (!Requires("towConversation.esp"))
		{
			#ifdef _DEBUG
			_MESSAGE("ERROR: towConversation.esp undetected.");
			#endif
			return false;
		}

		_MESSAGE("towConversation.dll");

		// SKSEのバージョンが1.7.1未満なら、プラグインをロードしない
		if (!Requires(kSKSEVersion_1_7_1))
		{
			#ifdef _DEBUG
			_MESSAGE("ERROR: your skse version is too old");
			#endif
			return false;
		}
		
		// 各々のインタフェースが古い場合は、プラグインをロードしない
		if (!Requires(SKSEScaleformInterface::kInterfaceVersion, SKSEPapyrusInterface::kInterfaceVersion))
		{
			#ifdef _DEBUG
			_MESSAGE("ERROR: interfaces are too old");
			#endif
			return false;
		}

		// プラグインの名前とバージョンを設定（必要）
		SetName("towConversation.dll");
		SetVersion(1);

		// インスタンスの初期化に成功
		return true;
	}

	//====================================================
	// プラグインが正しくロードされた時に呼ばれる
	// SKSEPlugin_Load() に相当
	//====================================================
	virtual bool OnLoad()
	{
		#ifdef _DEBUG
		_MESSAGE("skse plugin loaded.");
		#endif

		SKSEPapyrusInterface *papyrus = GetInterface<SKSEPapyrusInterface>();
		papyrus->Register(PapyrusFunction::Register);

		return true;
	}

	//====================================================
	// すべてのMODが読み込まれた後に呼ばれる
	//====================================================
	virtual void OnModLoaded()
	{
		DataHandler* dhnd = DataHandler::GetSingleton();
		const ModInfo* modInfo = dhnd->LookupModByName("towConversation.esp");
		if (modInfo != NULL)
		{
			CameraHook::Init();
		}
	}

} thePlugin;
