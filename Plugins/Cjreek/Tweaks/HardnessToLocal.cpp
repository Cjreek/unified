#include "nwnx.hpp"

#include "API/CNWSPlaceable.hpp"
#include "API/CNWSDoor.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;


static BOOL PlaceableLoadFromTemplateHook(CNWSPlaceable* pPlaceable, CResRef cResRef, CExoString* pTag);
static BOOL LoadPlaceableHook(CNWSPlaceable* pPlaceable, CResGFF * pRes, CResStruct * cPlaceableStruct, BOOL bLoadOID, CExoString * pTag);
static BOOL LoadDoorHook(CNWSDoor* pDoor, CResGFF * pRes, CResStruct * cItemStruct);

static Hooks::Hook s_PlaceableLoadFromTemplateHook = nullptr;
static Hooks::Hook s_LoadPlaceableHook = nullptr;
static Hooks::Hook s_LoadDoorHook = nullptr;

void HardnessToLocal() __attribute__((constructor));
void HardnessToLocal()
{
    s_PlaceableLoadFromTemplateHook = Hooks::HookFunction(&CNWSPlaceable::LoadFromTemplate, &PlaceableLoadFromTemplateHook, Hooks::Order::Late);
    s_LoadPlaceableHook = Hooks::HookFunction(&CNWSPlaceable::LoadPlaceable, &LoadPlaceableHook, Hooks::Order::Late);
    s_LoadDoorHook = Hooks::HookFunction(&CNWSDoor::LoadDoor, &LoadDoorHook, Hooks::Order::Late);
}

static BOOL PlaceableLoadFromTemplateHook(CNWSPlaceable* pPlaceable, CResRef cResRef, CExoString* pTag)
{
    auto result = s_PlaceableLoadFromTemplateHook->CallOriginal<BOOL>(pPlaceable, cResRef, pTag);

    if (pPlaceable->m_nHardness != 0)
    {
        CExoString hardnessVar = CExoString("HARDNESS");
        pPlaceable->m_ScriptVars.SetInt(hardnessVar, pPlaceable->m_nHardness);
        pPlaceable->m_nHardness = 0;
    }

    return result;
}

static BOOL LoadPlaceableHook(CNWSPlaceable* pPlaceable, CResGFF* pRes, CResStruct * cPlaceableStruct, BOOL bLoadOID, CExoString * pTag = nullptr)
{
    auto result = s_LoadPlaceableHook->CallOriginal<BOOL>(pPlaceable, pRes, cPlaceableStruct, bLoadOID, pTag);

    if (pPlaceable->m_nHardness != 0)
    {
        CExoString hardnessVar = CExoString("HARDNESS");
        pPlaceable->m_ScriptVars.SetInt(hardnessVar, pPlaceable->m_nHardness);
        pPlaceable->m_nHardness = 0;
    }

    return result;
}

static BOOL LoadDoorHook(CNWSDoor* pDoor, CResGFF* pRes, CResStruct* cItemStruct)
{
    auto result = s_LoadDoorHook->CallOriginal<BOOL>(pDoor, pRes, cItemStruct);

    if (pDoor->m_nHardness != 0)
    {
        CExoString hardnessVar = CExoString("HARDNESS");
        pDoor->m_ScriptVars.SetInt(hardnessVar, pDoor->m_nHardness);
        pDoor->m_nHardness = 0;
    }

    return result;
}