#include "StdAfx.h"

#include "CPhysicsKeyParser.h"
#include "CPhysicsSurfaceProps.h"

extern CPhysicsSurfaceProps g_SurfaceDatabase;

static void ReadVector(const char *pString, Vector& out)
{
	float x, y, z;
	sscanf(pString, "%f %f %f", &x, &y, &z);
	out.x = x;
	out.y = y;
	out.z = z;
}

static void ReadVector4D(const char *pString, Vector4D& out)
{
	float x, y, z, w;
	sscanf(pString, "%f %f %f %f", &x, &y, &z, &w);
	out.x = x;
	out.y = y;
	out.z = z;
	out.w = w;
}

CPhysicsKeyParser::CPhysicsKeyParser(const char *pKeyValues)
{
	m_pKeyValues = new KeyValues("CPhysicsKeyParser");
	m_pKeyValues->LoadFromBuffer("CPhysicsKeyParser", pKeyValues);
	m_pCurrentBlock = m_pKeyValues;
}

CPhysicsKeyParser::~CPhysicsKeyParser()
{
	if (m_pKeyValues)
		m_pKeyValues->deleteThis();
}

void CPhysicsKeyParser::NextBlock(void)
{
	if (m_pCurrentBlock)
		m_pCurrentBlock = m_pCurrentBlock->GetNextKey();
}

const char *CPhysicsKeyParser::GetCurrentBlockName(void)
{
	if (m_pCurrentBlock)
	{
		return m_pCurrentBlock->GetName();
	}
	return NULL;
}

bool CPhysicsKeyParser::Finished(void)
{
	return m_pCurrentBlock == NULL;
}

void CPhysicsKeyParser::ParseSolid(solid_t *pSolid, IVPhysicsKeyHandler *unknownKeyHandler)
{
	if (unknownKeyHandler)
		unknownKeyHandler->SetDefaults(pSolid);
	else
		memset(pSolid, 0, sizeof*pSolid);

	for (KeyValues* data = m_pCurrentBlock->GetFirstSubKey(); data; data = data->GetNextKey()) {
		const char* key = data->GetName();
		if (!stricmp(key, "index"))
			pSolid->index = data->GetInt();
		else if (!stricmp(key, "surfaceprop"))
			strncpy(pSolid->surfaceprop, data->GetString(), sizeof pSolid->surfaceprop);
		else if (!stricmp(key, "name"))
			strncpy(pSolid->name, data->GetString(), sizeof pSolid->name);
		else if (!stricmp(key, "parent"))
			strncpy(pSolid->parent, data->GetString(), sizeof pSolid->name);
		else if (!stricmp(key, "surfaceprop"))
			strncpy(pSolid->surfaceprop, data->GetString(), sizeof pSolid->name);
		else if (!stricmp(key, "mass"))
			pSolid->params.mass = data->GetFloat();
		else if (!stricmp(key, "massCenterOverride"))
			ReadVector(data->GetString(), pSolid->massCenterOverride);
		else if (!stricmp(key, "inertia"))
			pSolid->params.inertia = data->GetFloat();
		else if (!stricmp(key, "damping"))
			pSolid->params.damping = data->GetFloat();
		else if (!stricmp(key, "rotdamping"))
			pSolid->params.rotdamping = data->GetFloat();
		else if (!stricmp(key, "volume"))
			pSolid->params.volume = data->GetFloat();
		else if (!stricmp(key, "drag"))
			pSolid->params.dragCoefficient = data->GetFloat();
		//else if (!stricmp(key, "rollingdrag")) // This goes to pSolid->params.rollingDrag in the 2003 source but I cant find it in here
		else if (unknownKeyHandler)
			unknownKeyHandler->ParseKeyValue(pSolid, key, data->GetString());
	}
	NextBlock();
}

void CPhysicsKeyParser::ParseFluid(fluid_t *pFluid, IVPhysicsKeyHandler *unknownKeyHandler)
{
	if (unknownKeyHandler)
		unknownKeyHandler->SetDefaults(pFluid);
	else
		memset(pFluid, 0, sizeof*pFluid);

	for (KeyValues* data = m_pCurrentBlock->GetFirstSubKey(); data; data = data->GetNextKey()) {
		const char* key = data->GetName();
		if (!stricmp(key, "index"))
			pFluid->index = data->GetInt();
		else if (!stricmp(key, "surfaceprop"))
			strncpy(pFluid->surfaceprop, data->GetString(), sizeof pFluid->surfaceprop);
		else if (!stricmp(key, "contents"))
			pFluid->params.contents = data->GetInt();
		//if (!stricmp(key, "density")) // In the 2003 leak this existed, in the current code pFluid->params.density does not
		else if (!stricmp(key, "damping"))
			pFluid->params.damping = data->GetFloat();
		else if (!stricmp(key, "surfaceplane"))
			ReadVector4D(data->GetString(), pFluid->params.surfacePlane);
		else if (!stricmp(key, "currentvelocity"))
			ReadVector(data->GetString(), pFluid->params.currentVelocity);
		else if (unknownKeyHandler)
			unknownKeyHandler->ParseKeyValue(pFluid, key, data->GetString());
	}
	NextBlock();
}

void CPhysicsKeyParser::ParseRagdollConstraint(constraint_ragdollparams_t *pConstraint, IVPhysicsKeyHandler *unknownKeyHandler)
{
	if (unknownKeyHandler)
		unknownKeyHandler->SetDefaults(pConstraint);
	else
	{
		memset(pConstraint, 0, sizeof*pConstraint);
		pConstraint->childIndex = -1;
		pConstraint->parentIndex = -1;
	}

	for (KeyValues* data = m_pCurrentBlock->GetFirstSubKey(); data; data = data->GetNextKey()) {
		const char* key = data->GetName();
		if (!stricmp(key, "parent"))
			pConstraint->parentIndex = data->GetInt();
		if (!stricmp(key, "child"))
			pConstraint->childIndex = data->GetInt();
		else if (!stricmp(key, "xmin"))
			pConstraint->axes[0].minRotation = data->GetFloat();
		else if (!stricmp(key, "xmax"))
			pConstraint->axes[0].maxRotation = data->GetFloat();
		else if (!stricmp(key, "xfriction"))
		{
			pConstraint->axes[0].angularVelocity = 0;
			pConstraint->axes[0].torque = data->GetFloat();
		}
		else if (!stricmp(key, "ymin"))
			pConstraint->axes[1].minRotation = data->GetFloat();
		else if (!stricmp(key, "ymax"))
			pConstraint->axes[1].maxRotation = data->GetFloat();
		else if (!stricmp(key, "yfriction"))
		{
			pConstraint->axes[1].angularVelocity = 0;
			pConstraint->axes[1].torque = data->GetFloat();
		}
		else if (!stricmp(key, "zmin"))
			pConstraint->axes[2].minRotation = data->GetFloat();
		else if (!stricmp(key, "zmax"))
			pConstraint->axes[2].maxRotation = data->GetFloat();
		else if (!stricmp(key, "zfriction"))
		{
			pConstraint->axes[2].angularVelocity = 0;
			pConstraint->axes[2].torque = data->GetFloat();
		}
		else if (unknownKeyHandler)
			unknownKeyHandler->ParseKeyValue(pConstraint, key, data->GetString());
	}
	NextBlock();
}

void CPhysicsKeyParser::ParseSurfaceTable(int *table, IVPhysicsKeyHandler *unknownKeyHandler)
{
	for (KeyValues* data = m_pCurrentBlock->GetFirstSubKey(); data; data = data->GetNextKey()) {
		if (data->GetInt() < 128)
			table[data->GetInt()] = g_SurfaceDatabase.GetSurfaceIndex(data->GetName());
	}
	NextBlock();
}

void CPhysicsKeyParser::ParseCustom(void *pCustom, IVPhysicsKeyHandler *unknownKeyHandler)
{
	if (unknownKeyHandler)
		unknownKeyHandler->SetDefaults(pCustom);
	for (KeyValues* data = m_pCurrentBlock->GetFirstSubKey(); data; data = data->GetNextKey()) {
		const char* key = data->GetName();
		const char* value = data->GetString();
		if (unknownKeyHandler)
			unknownKeyHandler->ParseKeyValue(pCustom, key, value);
	}
	NextBlock();
}

void CPhysicsKeyParser::ParseVehicle(vehicleparams_t *pVehicle, IVPhysicsKeyHandler *unknownKeyHandler)
{
	if (unknownKeyHandler)
		unknownKeyHandler->SetDefaults(pVehicle);
	else
		memset(pVehicle, 0, sizeof*pVehicle);

	for (KeyValues* data = m_pCurrentBlock->GetFirstSubKey(); data; data = data->GetNextKey()) {
		const char* key = data->GetName();
		if (!stricmp(key, "axle"))
		{
			ParseVehicleAxle(pVehicle, data);
			pVehicle->axleCount++;
		}
		else if (!stricmp(key, "body"))
			ParseVehicleBody(pVehicle, data);
		else if (!stricmp(key, "engine"))
			ParseVehicleEngine(pVehicle, data);
		else if (!stricmp(key, "steering"))
			ParseVehicleSteering(pVehicle, data);
		else if (!stricmp(key, "wheelsperaxle"))
			pVehicle->wheelsPerAxle = data->GetInt();
	}
	NextBlock();
}


void CPhysicsKeyParser::ParseVehicleAxle(vehicleparams_t *pVehicle, KeyValues *kv)
{
	NOT_IMPLEMENTED;
}

void CPhysicsKeyParser::ParseVehicleBody(vehicleparams_t *pVehicle, KeyValues *kv)
{
	NOT_IMPLEMENTED;
}

void CPhysicsKeyParser::ParseVehicleEngine(vehicleparams_t *pVehicle, KeyValues *kv)
{
	NOT_IMPLEMENTED;
}

void CPhysicsKeyParser::ParseVehicleSteering(vehicleparams_t *pVehicle, KeyValues *kv)
{
	NOT_IMPLEMENTED;
}
