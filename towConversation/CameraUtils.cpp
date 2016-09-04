#include <skse.h>
#include <skse/GameData.h>
#include <skse/GameRTTI.h>
#include <skse/GameReferences.h>
#include <skse/GameCamera.h>
#include <skse/NiNodes.h>

#include "Address.h"
#include "CameraUtils.h"

void GetAngle(const NiPoint3 &from, const NiPoint3 &to, AngleZX* angle)
{
	float x = to.x - from.x;
	float y = to.y - from.y;
	float z = to.z - from.z;
	float xy = sqrt(x*x + y*y);

	angle->z = atan2(x, y);
	angle->x = atan2(-z, xy);
	angle->distance = sqrt(xy*xy + z*z);
}


bool GetAngle(TESObjectREFR* target, AngleZX* angle)
{
	NiPoint3 targetPos;
	NiPoint3 cameraPos;

	if (!GetTargetPos(target, &targetPos))
		return false;
	GetCameraPos(&cameraPos);

	GetAngle(cameraPos, targetPos, angle);
	
	return true;
}

// �v���C���[�̃J�����ʒu���擾
void GetCameraPos(NiPoint3* pos)
{
	PlayerCharacter* player = *g_thePlayer;
	PlayerCamera* camera = PlayerCamera::GetSingleton();
	float x, y, z;

	FirstPersonState* fps = (FirstPersonState*)camera->cameraStates[camera->kCameraState_FirstPerson];
	ThirdPersonState* tps = (ThirdPersonState*)camera->cameraStates[camera->kCameraState_ThirdPerson2];
	if (camera->cameraState == fps || camera->cameraState == tps)
	{
		NiNode* node = camera->cameraNode;
		if (node)
		{
			x = node->m_worldTransform.pos.x;
			y = node->m_worldTransform.pos.y;
			z = node->m_worldTransform.pos.z;
			/*
			NiAVObject* obj = node->m_children.m_data[0];
			if (node->m_children.m_size > 0 && node->m_children.m_data)
			{
				NiAVObject* obj = node->m_children.m_data[0];
				if (obj)
				{
					x = obj->m_worldTransform.pos.x;
					y = obj->m_worldTransform.pos.y;
					z = obj->m_worldTransform.pos.z;
				}
			}
			*/
		}
	}
	else
	{
		NiPoint3 playerPos;

		player->GetMarkerPosition(&playerPos);
		z = player->pos.z;
		x = player->pos.x;
		y = player->pos.y;
	}
	
	pos->x = x;
	pos->y = y;
	pos->z = z;
}

// �A�N�^�[�̃m�[�h�̃��[���h���W���擾
bool GetNodePosition(Actor* actor, const char* nodeName, NiPoint3* point)
{
	bool bResult = false;

	if (nodeName[0])
	{
		BSFixedString NeckNodeName = "NPC Neck [Neck]";
		NiAVObject* object = actor->GetNiNode();
		if (object)
		{
			object = object->GetObjectByName(&NeckNodeName.data);
			if (object)
			{
				point->x = object->m_worldTransform.pos.x;
				point->y = object->m_worldTransform.pos.y;
				point->z = object->m_worldTransform.pos.z;
				bResult = true;
			}
		}
	}

	return bResult;
}

// �A�N�^�[��Torso(��)�̈ʒu���擾
bool GetTorsoPos(Actor* actor, NiPoint3* point)
{
	TESRace* race = actor->race;
	BGSBodyPartData* bodyPart = race->bodyPartData;
	BGSBodyPartData::Data* data;

	// bodyPart->part[0] ��
	// bodyPart->part[1] ��
	data = bodyPart->part[0];
	if (data)
	{
		return GetNodePosition(actor, data->unk04.data, point);
	}

	return false;
}


// �^�[�Q�b�g�̍��W���擾����
bool GetTargetPos(TESObjectREFR* target, NiPoint3* pos)
{
	if (target->GetNiNode() == NULL)
		return false;
	
	if (target->formType == kFormType_Character)
	{
		if (!GetTorsoPos((Actor*)target, pos))
			target->GetMarkerPosition(pos);
	}
	else
	{
		pos->x = target->pos.x;
		pos->y = target->pos.y;
		pos->z = target->pos.z;
	}
	
	return true;
}

