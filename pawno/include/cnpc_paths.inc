/*
*	Created:			05.10.12
*	Author:				009
*	Last Modifed:		07.10.12
*	Description:		NPC paths manager (Controllable NPC)
*/


#if defined _cnpc_paths_included
	#endinput
#endif

#define _cnpc_paths_included
#pragma library cnpc_paths

// --------------------------------------------------
// includes
// --------------------------------------------------
#include <cnpc>

// --------------------------------------------------
// defines
// --------------------------------------------------


// --------------------------------------------------
// enums
// --------------------------------------------------
enum
{
	PATH_STATE_NOT_EXIST,
	PATH_STATE_CALCULATING,
	PATH_STATE_CALCULATED,
	PATH_STATE_NEED_DESTROY
};

// --------------------------------------------------
// statics
// --------------------------------------------------
static
	path_for[MAX_NODE_PATHS],		// ������ �� NPC ��� ������� ������� ����
	path_type[MAX_NODE_PATHS char],	// ��� ���������� ���� (���������� ���� ������������)
	path_state[MAX_NODE_PATHS char],// ��������� ����
	npc_use[MAX_NPCS];				// ������ �� ���� ������� ��������� ��� NPC
	
// --------------------------------------------------
// forwards
// --------------------------------------------------
forward OnStartCalcPathForNPC(npcid);
forward OnCompleteCalcPathForNPC(npcid);

// --------------------------------------------------
// stocks
// --------------------------------------------------
/*
	Name: 
		paths_ResetData(npcid)
	Description:
		�������� ������ ������ ��� NPC
	Params:
		npcid - id NPC
	Return:
		-
*/
stock paths_ResetData(npcid)
{
	if(npc_use[npcid] != INVALID_NODE_PATH)
	{
		if(path_state{ npc_use[npcid] } == PATH_STATE_CALCULATED)
		{
			path_Destroy(npc_use[npcid]);
			path_state{ npc_use[npcid] } = PATH_STATE_NOT_EXIST;
		}
		else path_state{ npc_use[npcid] } = PATH_STATE_NEED_DESTROY;
		
		path_for[ npc_use[npcid] ] = INVALID_PLAYER_ID;
		path_type{ npc_use[npcid] } = 0;
		npc_use[npcid] = INVALID_NODE_PATH;
	}
}

/*
	Name: 
		paths_CalcPathForNPC(npcid,nodetype,startnodeid,endnodeid)
	Description:
		������ ���� ��� NPC (��������� ���� ������������� ����)
	Params:
		npcid - id NPC
		nodetype - ��� ������������� ����� (���������� ���� ������������)
		startnodeid - id ������� ����� ������ ����
		endnodeid - id ������� ����� ����� ����
	Return:
		-
*/
stock paths_CalcPathForNPC(npcid,nodetype,startnodeid,endnodeid)
{
	// ��������� ��������� �� ����� ���� ���� �� ������ NPC
	if(npc_use[npcid] != INVALID_NODE_PATH)
	{
		// ������ ������ ����
		paths_ResetData(npcid);
	}
	// �������� ��� ������� ��� ������
	new areas[64];
	for(new i = 0;i < 64;i++) areas[i] = 1;
	// �������� ������ ����
	new pathid = dijkstra_CalcPathByNodes(areas,nodetype,startnodeid,endnodeid);
	// ���������� ������
	npc_use[npcid] = pathid;
	path_for[pathid] = npcid;
	path_type{pathid} = nodetype;
	path_state{pathid} = PATH_STATE_CALCULATING;
	// �������� ������
	OnStartCalcPathForNPC(npcid);
}

/*
	Name: 
		paths_GetPathLen(npcid)
	Description:
		�������� ����� ���� (���������� �����) ���������� ��� NPC
	Params:
		npcid - id NPC
	Return:
		int
*/
stock paths_GetPathLen(npcid)
{
	// ��������� ��������� �� ����� ���� ���� �� ������ NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0;
	
	return path_GetPathLen(npc_use[npcid]);
}

/*
	Name: 
		paths_GetPathDist(npcid)
	Description:
		�������� ���������� ���� ���������� ��� NPC
	Params:
		npcid - id NPC
	Return:
		float
*/
stock Float:paths_GetPathDist(npcid)
{
	// ��������� ��������� �� ����� ���� ���� �� ������ NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0.0;
	
	return path_GetPathDist(npc_use[npcid]);
}

/*
	Name: 
		paths_GetPathPointNodeid(npcid,pointid)
	Description:
		�������� nodeid ����� ����
	Params:
		npcid - id NPC
		pointid - id ����� �� ����
	Return:
		int
*/
stock paths_GetPathPointNodeid(npcid,pointid)
{
	// ��������� ��������� �� ����� ���� ���� �� ������ NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0;
	// �������� ���������� ����� ����
	new len = path_GetPathLen(npc_use[npcid]);
	// ��������� �������������� ����� ����
	if((pointid < 0) || (pointid >= len)) return 0;
	// �������� ����� ����� ����
	return path_GetPathNodeid(npc_use[npcid],pointid);
}

/*
	Name: 
		paths_GetPathPointPos(npcid,pointid,&Float:x,&Float:y,&Float:z)
	Description:
		�������� ���������� ����� ����
	Params:
		npcid - id NPC
		pointid - id ����� �� ����
		x,y,z - ������������ ���������� �����
	Return:
		0 - ������ (��� ���� ���� ����� �� ����������� ����)
		1 - ���������� ��������
*/
stock paths_GetPathPointPos(npcid,pointid,&Float:x,&Float:y,&Float:z)
{
	// ��������� ��������� �� ����� ���� ���� �� ������ NPC
	if(npc_use[npcid] == INVALID_NODE_PATH) return 0;
	// �������� ���������� ����� ����
	new len = path_GetPathLen(npc_use[npcid]);
	// ��������� �������������� ����� ����
	if((pointid < 0) || (pointid >= len)) return 0;
	// �������� ����� ����� ����
	new nodeid = path_GetPathNodeid(npc_use[npcid],pointid);
	// �������� ���������� ����� ����
	nodes_GetNodePos(path_type{ npc_use[npcid] },nodeid,x,y,z);
	return 1;
}

// --------------------------------------------------
// obligatory functions
// --------------------------------------------------
paths_Init()
{
	// ������ ������
	for(new i = 0;i < MAX_NODE_PATHS;i++)
	{
		path_for[i] = INVALID_PLAYER_ID;
		path_type{i} = 0;
		path_state{i} = PATH_STATE_NOT_EXIST;
	}
	for(new i = 0;i < MAX_NPCS;i++) npc_use[i] = INVALID_NODE_PATH;
}

paths_Exit()
{
	// �������� ������ ���� npc
	for(new i = 0;i < MAX_NPCS;i++)
	{
		if(!IsValidNPC(i)) continue;
		
		paths_ResetData(i);
	}
}

paths_OnPathCalculated(pathid)
{
	// ��������� ��������� ����
	if(path_state{pathid} == PATH_STATE_NEED_DESTROY)
	{
		path_Destroy(pathid);
		path_state{pathid} = PATH_STATE_NOT_EXIST;
	}
	else if(path_state{pathid} == PATH_STATE_CALCULATING)
	{
		// �������� NPC ��� �������� ������ ����
		new npcid = path_for[pathid];
		// ��������� �������������� ���� � ������� ������
		if(npcid == INVALID_PLAYER_ID) return;
		// ��������� ������
		path_state{pathid} = PATH_STATE_CALCULATED;
		// �������� ������ � ���������� �������� ����
		OnCompleteCalcPathForNPC(npcid);
	}
}