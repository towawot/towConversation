#pragma once

extern bool g_Register;


class VMClassRegistry;
struct StaticFunctionTag;

namespace Papyrus
{ 
	Character* GetPlayerDialogueTarget(StaticFunctionTag*);
	void ForceFirstPersonSmooth(StaticFunctionTag*);
	void ForceThirdPersonSmooth(StaticFunctionTag*);
	void ForceThirdPersonEX(StaticFunctionTag*);
	void LookAtRef(StaticFunctionTag*, TESObjectREFR* akRef, float wait);
	void SetFOVSmooth(StaticFunctionTag*, float worldfov, float firstfov, float delay);
	void SetCameraSpeed(StaticFunctionTag*, float speed);

	void RegisterFuncs(VMClassRegistry* registry);
}
