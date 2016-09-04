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
	// �����Ńv���O�C���E�C���X�^���X�̏���������������
	// SKSEPlugin_Query() �ɑ���
	//====================================================
	virtual bool InitInstance()
	{
		// towConversation.esp �� MOD���X�g�ɖ������΁A�v���O�C�������[�h���Ȃ�
		if (!Requires("towConversation.esp"))
		{
			#ifdef _DEBUG
			_MESSAGE("ERROR: towConversation.esp undetected.");
			#endif
			return false;
		}

		_MESSAGE("towConversation.dll");

		// SKSE�̃o�[�W������1.7.1�����Ȃ�A�v���O�C�������[�h���Ȃ�
		if (!Requires(kSKSEVersion_1_7_1))
		{
			#ifdef _DEBUG
			_MESSAGE("ERROR: your skse version is too old");
			#endif
			return false;
		}
		
		// �e�X�̃C���^�t�F�[�X���Â��ꍇ�́A�v���O�C�������[�h���Ȃ�
		if (!Requires(SKSEScaleformInterface::kInterfaceVersion, SKSEPapyrusInterface::kInterfaceVersion))
		{
			#ifdef _DEBUG
			_MESSAGE("ERROR: interfaces are too old");
			#endif
			return false;
		}

		// �v���O�C���̖��O�ƃo�[�W������ݒ�i�K�v�j
		SetName("towConversation.dll");
		SetVersion(1);

		// �C���X�^���X�̏������ɐ���
		return true;
	}

	//====================================================
	// �v���O�C�������������[�h���ꂽ���ɌĂ΂��
	// SKSEPlugin_Load() �ɑ���
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
	// ���ׂĂ�MOD���ǂݍ��܂ꂽ��ɌĂ΂��
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
