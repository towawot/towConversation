#include <skse.h>
#include <skse/GameReferences.h>
#include <skse/GameForms.h>
#include <skse/GameCamera.h>
#include <skse/GamePapyrusFunctions.h>
#include <skse/PluginAPI.h>

#include "Address.h"
#include "CameraUtils.h"
#include "Papyrus.h"
#include "CameraHook.h"

#define _USE_MATH_DEFINES
#include <math.h>


// プレイヤーの角度を変更する
static void SetPlayerAngle(float rotZ, float rotX, float wait)
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
void LookAt(float posX, float posY, float posZ, float wait)
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

	SetPlayerAngle(rotZ, rotX, wait);
}

BGSKeyword* GetKeywordByID(UInt32 formID)
{
	BGSKeyword* key = NULL;
	if (formID)
		key = (BGSKeyword*)LookupFormByID(formID);
	return key;
}

namespace CameraFunction
{
	void ForceFirstPersonSmooth(void)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		if (camera->cameraState->stateId == PlayerCamera::kCameraState_ThirdPerson2)
		{
			ThirdPersonState* state = (ThirdPersonState*)camera->cameraState;
			if (state->unkB5 == 0)
			{
				state->unk64   = state->curPosY;
				state->dstPosY = -0.2f;
				state->unkB5 = 1;
			}
			else
			{
				state->curPosY = state->dstPosY;
			}
		}
	}

	void ForceThirdPersonInstant(void)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		if (camera->cameraState == NULL || camera->cameraState->stateId < PlayerCamera::kCameraState_ThirdPerson2)
		{
			ThirdPersonState* tps = (ThirdPersonState*)camera->cameraStates[PlayerCamera::kCameraState_ThirdPerson2];

			tps->basePosX = tps->fOverShoulderPosX;
			tps->basePosY = tps->fOverShoulderCombatAddY;
			tps->basePosZ = tps->fOverShoulderPosZ;
			tps->curPosY  = tps->dstPosY;

			CALL_MEMBER_FN(camera, SetCameraState)(tps);
		}
	}

	void ForceThirdPersonSmooth(void)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		if (camera->cameraState == NULL || camera->cameraState->stateId < PlayerCamera::kCameraState_ThirdPerson2)
		{
			ThirdPersonState* tps = (ThirdPersonState*)camera->cameraStates[PlayerCamera::kCameraState_ThirdPerson2];

			tps->basePosX = tps->fOverShoulderPosX;
			tps->basePosY = tps->fOverShoulderCombatAddY;
			tps->basePosZ = tps->fOverShoulderPosZ;
			tps->curPosY  = -0.2f;
			tps->dstPosY  = tps->unk64;

			CALL_MEMBER_FN(camera, SetCameraState)(tps);
		}
	}
};

// パピルス関数本体
class aaatowCV
{
public:
	// 一人称視点に切り替え
	static void ForceFirstPersonSmooth()
	{
		CameraFunction::ForceFirstPersonSmooth();
	}

	// 三人称視点に切り替え
	static void ForceThirdPersonSmooth()
	{
		CameraFunction::ForceThirdPersonSmooth();
	}

	// 三人称視点に切り替えEX
	static void ForceThirdPersonEX()
	{
		CameraFunction::ForceThirdPersonInstant();
	}

	static bool IsFirstPerson()
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		return (camera->cameraState == camera->cameraStates[camera->kCameraState_FirstPerson]);
	}
	
	// FOVをスムーズに変更
	static void SetFOVSmooth(float worldfov, float firstfov, float delay)
	{
		if (worldfov <= 0.0)
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
	static void SetCameraSpeed(float speed)
	{
		g_cameraSpeed = speed;
	}

	// プレイヤーを対象に向ける
	static void LookAtRef(TESObjectREFR* akRef, float wait)
	{
		if (!akRef) // && akRef->formType != kFormType_Character)
			return;
		if (akRef->GetNiNode() == NULL)
			return;

		NiPoint3 targetPos;
		if (!GetTargetPos(akRef, &targetPos))
			return;

		refTarget = akRef;
		LookAt(targetPos.x, targetPos.y, targetPos.z, wait);
	}
	
	// プレイヤーを対象に向ける
	static void LookAtActor(TESObjectREFR* ref, float wait)
	{
		if (!ref)
			return;
		if (ref->GetNiNode() == NULL)
			return;
		if (ref->formType != kFormType_Character)
			return;

		NiPoint3 targetPos;

		if (!GetTargetPos(ref, &targetPos))
			return;

		refTarget = ref;
		LookAt(targetPos.x, targetPos.y, targetPos.z, wait);
	}
	
	// プレイヤーを対象に向ける
	static float GetActorDistance(TESObjectREFR* akRef)
	{
		if (!akRef)
			return 0;
		if (akRef->GetNiNode() == NULL)
			return 0;
		if (akRef->formType != kFormType_Character)
			return 0;

		PlayerCharacter* player = *g_thePlayer;

		double x, y, z, xyz;
		NiPoint3 playerPos, targetPos;

		if (!GetTargetPos(akRef, &targetPos))
			return 0;

		GetTargetPos(player, &playerPos);
		x = targetPos.x - playerPos.x;
		y = targetPos.y - playerPos.y;
		z = targetPos.z - playerPos.z;
		xyz = sqrt(x*x + y*y + z*z);
		return xyz;
	}
	
	static bool IsDragonType(Actor* actor)
	{
		UInt32 formID = 0x035D59; //actorTypeDragon
		BGSKeyword* key = GetKeywordByID(formID);
		return (key && actor && actor->HasKeyword(key));
	}
	
	static void PapyrusTrace(BSFixedString s)
	{
		#ifdef _DEBUG
			_MESSAGE("%s", s);
		#endif
	}
};

namespace PapyrusFunction
{
	bool Register(VMClassRegistry* registry)
	{
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, ForceFirstPersonSmooth, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, ForceThirdPersonSmooth, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, ForceThirdPersonEX, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, SetFOVSmooth, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, SetCameraSpeed, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, LookAtRef, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, LookAtActor, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, GetActorDistance, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, IsDragonType, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, IsFirstPerson, registry);
		REGISTER_PAPYRUS_FUNCTION(aaatowCV, PapyrusTrace, registry);

		return true;
	}

}
