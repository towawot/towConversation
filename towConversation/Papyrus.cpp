#include <skse.h>
#include <skse/GameReferences.h>
#include <skse/GameForms.h>
#include <skse/GameCamera.h>
#include <skse/GameObjects.h>
#include <skse/PapyrusVM.h>
#include <skse/PapyrusNativeFunctions.h>
#include <skse/NiNodes.h>

#include "Papyrus.h"
#include "CameraHook.h"
#include "Utils.h"
#include "Address.h"

#define _USE_MATH_DEFINES
#include <math.h>

bool g_Register = false;

class TESPlayerDialogueTarget
{
public:
	MEMBER_FN_PREFIX(TESPlayerDialogueTarget);
	DEFINE_MEMBER_FN(GetTarget, Character*, ADDR_GetTarget);

	Character* GetTarget(void)
	{
		return CALL_MEMBER_FN(this, GetTarget)();
	}
};

static TESPlayerDialogueTarget** g_playerDialogueTarget = (TESPlayerDialogueTarget**)ADDR_g_playerDialogueTarget;

// パピルス関数本体
namespace Papyrus
{
	static const char s_className[] ="aaatowCV_Main";

	// プレイヤーがダイアログ会話中の相手を探す
	Character* GetPlayerDialogueTarget(StaticFunctionTag*)
	{
		Character* target = (*g_playerDialogueTarget)->GetTarget();
		if (!target || target->formType != kFormType_Character)
		{
			target = NULL;
		}
		return target;
	}

	// 一人称視点に切り替え
	void ForceFirstPersonSmooth(StaticFunctionTag*)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		camera->ForceFirstPersonSmooth();
	}

	// 三人称視点に切り替え
	void ForceThirdPersonSmooth(StaticFunctionTag*)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		camera->ForceThirdPersonSmooth();
	}

	// 三人称視点に切り替えEX
	void ForceThirdPersonEX(StaticFunctionTag*)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		camera->ForceThirdPersonEX();
	}

	// プレイヤーの角度を変更する
	void SetPlayerAngle(StaticFunctionTag*, float rotZ, float rotX, float wait)
	{
		PlayerCharacter* player = *g_thePlayer;
		TESCameraController* controller = TESCameraController::GetSingleton();

		if (wait < 20)
		{
			wait = 20;
		}

		controller->Rotate(player->rot.z, player->rot.x, rotZ, rotX, wait, 0);
	}
	
	// カメラを任意の座標に向ける
	void LookAt(StaticFunctionTag*, float posX, float posY, float posZ, float wait)
	{
		PlayerCharacter* player = *g_thePlayer;
		NiPoint3 cameraPos;
		double x, y, z, xy;
		double rotZ, rotX;
		
		GetCameraPos(&cameraPos);

		x = posX - cameraPos.x;
		y = posY - cameraPos.y;
		z = posZ - cameraPos.z;

		xy = sqrt(x*x + y*y);
		rotZ = atan2(x, y);
		rotX = atan2(-z, xy);
		
		if (rotZ - player->rot.z > M_PI)
			rotZ -= M_PI * 2;
		else if (rotZ - player->rot.z < -M_PI)
			rotZ += M_PI * 2;

		SetPlayerAngle(NULL, rotZ, rotX, wait);
	}

	// プレイヤーを対象に向ける
	void LookAtRef(StaticFunctionTag*, TESObjectREFR* akRef, float wait)
	{
		if (akRef && akRef->formType != kFormType_Character)
			return;
		
		if (akRef == NULL)
			return;
		// akRef->Is3DLoaded()
		if (akRef->GetNiNode() == NULL)
			return;

		NiPoint3 targetPos;
		if (!GetTargetPos(akRef, &targetPos))
			return;

		refTarget = akRef;
		LookAt(NULL, targetPos.x, targetPos.y, targetPos.z, wait);
	}

	// FOVをスムーズに変更
	void SetFOVSmooth(StaticFunctionTag*, float worldfov, float firstfov, float delay)
	{
		if (worldfov <= 0)
			return;

		if (delay <= 0.0)
		{
			g_worldfovTo = worldfov;
			g_firstfovTo = firstfov;
			g_fovStep = 1;
		}
		else
		{
			int step = delay * 60 / 1000;
			if (step <= 0) step = 1;
			g_worldfovTo = worldfov;
			g_firstfovTo = firstfov;
			g_fovStep = step;
		}
	}
	
	// カメラのスピードを変更
	void SetCameraSpeed(StaticFunctionTag*, float speed)
	{
		g_cameraSpeed = speed;
	}
	

	// パピルス関数を登録するときに呼ばれる
	void RegisterFuncs(VMClassRegistry* registry)
	{
	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, void, TESObjectREFR*, float>("LookAtRef", s_className, LookAtRef, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("ForceFirstPersonSmooth", s_className, ForceFirstPersonSmooth, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("ForceThirdPersonSmooth", s_className, ForceThirdPersonSmooth, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("ForceThirdPersonEX", s_className, ForceThirdPersonEX, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, Character*>("GetPlayerDialogueTarget", s_className, GetPlayerDialogueTarget, registry));
	registry->RegisterFunction(
		new NativeFunction3<StaticFunctionTag, void, float, float, float>("SetFOVSmooth", s_className, SetFOVSmooth, registry));
 	registry->RegisterFunction(
 		new NativeFunction1<StaticFunctionTag, void, float>("SetCameraSpeed", s_className, SetCameraSpeed, registry));
	}

}
