
#pragma once

struct AngleZX
{
	double  z;
	double  x;
	double  distance;
};

class TESObjectREFR;
class Actor;
class NiPoint3;

void GetAngle(const NiPoint3 &from, const NiPoint3 &to, AngleZX* angle);
bool GetAngle(TESObjectREFR* target, AngleZX* angle);
void GetCameraPos(NiPoint3* pos);
bool GetTargetPos(TESObjectREFR* target, NiPoint3* pos);
bool GetNodePosition(Actor* actor, const char* nodeName, NiPoint3* point);
