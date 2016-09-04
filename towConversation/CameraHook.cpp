#include <skse.h>
#include <skse/GameReferences.h>
#include <skse/GameCamera.h>

#include "Address.h"
#include "CameraUtils.h"
#include "Papyrus.h"
#include "CameraHook.h"

#define _USE_MATH_DEFINES
#include <math.h>

float g_cameraSpeed = 700.0;
float g_worldfovTo;;
float g_firstfovTo;
int   g_fovStep;
TESObjectREFR* refTarget = NULL;

#define DEFINE_MEMBER_FN_EX(className, functionName, retnType, address, ...)		\
	typedef retnType (className::* _##functionName##_type)(__VA_ARGS__);			\
																					\
	inline _##functionName##_type * _##functionName##_GetPtr(void)					\
	{																				\
		static const UInt32 _address = address;										\
		return (_##functionName##_type *)&_address;									\
	}

#define CALL_MEMBER_FN_EX(obj, fn)	\
	((*(obj)).*(*(_##fn##_GetPtr())))


DEFINE_MEMBER_FN_EX(PlayerCharacter, SetAngleZ, void, ADDR_SetAngleZ, float)
DEFINE_MEMBER_FN_EX(PlayerCharacter, SetAngleX, void, ADDR_SetAngleX, float)

static bool CalcAngle(TESObjectREFR* target, AngleZX* angle)
{
	AngleZX baseAngle;
	if (!GetAngle(target, &baseAngle))
		return false;
	
	PlayerCharacter* player = *g_thePlayer;

	double angleDiffZ, angleDiffX;
	{
		angleDiffZ = baseAngle.z - (double)player->rot.z;
		angleDiffX = baseAngle.x - (double)player->rot.x;

		PlayerCamera* camera = PlayerCamera::GetSingleton();
		if (camera->cameraState == camera->cameraStates[camera->kCameraState_ThirdPerson2])
		{
			ThirdPersonState* tps = (ThirdPersonState*)camera->cameraState;
			if (tps->unkB4 != 0)
			{
				angleDiffZ -= (double)tps->diffRotZ;
				angleDiffX -= (double)tps->diffRotX;
			}
		}

		while (angleDiffZ < -M_PI)
			angleDiffZ += 2*M_PI;
		while (angleDiffZ > M_PI)
			angleDiffZ -= 2*M_PI;
		while (angleDiffX < -M_PI)
			angleDiffX += 2*M_PI;
		while (angleDiffX > M_PI)
			angleDiffX -= 2*M_PI;
	}
	angleDiffZ = angleDiffZ / (g_cameraSpeed * 60 / 2000);
	angleDiffX = angleDiffX / (g_cameraSpeed * 60 / 2000);

	
	double angleZ, angleX;
	{
		angleZ = (double)player->rot.z + angleDiffZ;
		angleX = (double)player->rot.x + angleDiffX;
	
		while (angleZ < 0)
			angleZ += 2*M_PI;
		while (angleZ > 2*M_PI)
			angleZ -= 2*M_PI;
		while (angleX < -M_PI)
			angleX += 2*M_PI;
		while (angleX > M_PI)
			angleX -= 2*M_PI;
	}
	
	angle->z = angleZ;
	angle->x = angleX;

	return true;
}

static void RotateCamera(TESObjectREFR* target)
{
	if (TESCameraController::GetSingleton()->unk1C == 0)
		return;

	AngleZX angle;
	if (!CalcAngle(target, &angle))
		return;

	PlayerCharacter* player = *g_thePlayer;
	PlayerCamera* camera = PlayerCamera::GetSingleton();
	if ((player->actorState.flags04 & 0x0003C000) == 0)
	{
		if (camera->cameraState == camera->cameraStates[camera->kCameraState_ThirdPerson2])
		{
			
			ThirdPersonState* tps = (ThirdPersonState*)camera->cameraState;
			tps->diffRotZ = 0.0;
			tps->diffRotX = 0.0;
		}
		CALL_MEMBER_FN_EX(player, SetAngleZ)(angle.z);
		CALL_MEMBER_FN_EX(player, SetAngleX)(angle.x);
	}
	else
	{
		if (camera->cameraState == camera->cameraStates[camera->kCameraState_FirstPerson])
		{
			FirstPersonState* fps = (FirstPersonState*)camera->cameraState;
			angle.z -= player->Unk_A3(0);

			fps->unk20 = angle.z;
			CALL_MEMBER_FN_EX(player, SetAngleX)(angle.x);
		}
		else if (camera->cameraState == camera->cameraStates[camera->kCameraState_ThirdPerson2])
		{
			ThirdPersonState* tps = (ThirdPersonState*)camera->cameraState;
			angle.z -= camera->unkC4;

			tps->diffRotZ = angle.z;
			tps->diffRotX = 0;
		}
		else
		{
			CALL_MEMBER_FN_EX(player, SetAngleZ)(angle.z);
			CALL_MEMBER_FN_EX(player, SetAngleX)(angle.x);
		}
	}
}


static void OnCameraMove(UInt32* stack, UInt32 ecx)
{
	if (g_fovStep > 0)
	{
		PlayerCamera* camera = PlayerCamera::GetSingleton();
		if (g_fovStep == 1)
		{
			refTarget = NULL;
			camera->worldFOV = g_worldfovTo;;
			camera->firstPersonFOV = g_firstfovTo;
		}
		else
		{
			float diff = g_worldfovTo - camera->worldFOV;
			camera->worldFOV += diff / g_fovStep;
			float diff2 = g_firstfovTo - camera->firstPersonFOV;
			camera->firstPersonFOV += diff2 / g_fovStep;
			if(refTarget)
				RotateCamera(refTarget);
		}
		g_fovStep--;
	}
}


#include <skse/SafeWrite.h>
#include "HookUtil.h"

namespace CameraHook
{
	void Init(void)
	{
		HookRelCall(ADDR_OnCameraMove, OnCameraMove);
	}
}

