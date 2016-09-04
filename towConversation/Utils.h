
#pragma once

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


struct AngleZX
{
	double  z;
	double  x;
	double  distance;
};


class TESObjectREFR;
class TESQuest;
class BGSListForm;
class Actor;
class NiPoint3;

void GetAngle(const NiPoint3 &from, const NiPoint3 &to, AngleZX* angle);
bool GetAngle(TESObjectREFR* target, AngleZX* angle);
void GetCameraPos(NiPoint3* pos);
bool GetTargetPos(TESObjectREFR* target, NiPoint3* pos);
Actor* GetCombatTarget(Actor* actor);
bool IsPlayerTeammate(Actor* actor);
bool IsPlayerFollower(Actor* actor);
