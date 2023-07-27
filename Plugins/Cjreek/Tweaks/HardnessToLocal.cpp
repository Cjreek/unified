#include "nwnx.hpp"

#include "API/CNWSPlaceable.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;


static BOOL PlaceableLoadFromTemplateHook(CNWSPlaceable* pPlaceable, CResRef cResRef, CExoString* pTag);
static BOOL LoadPlaceableHook(CNWSPlaceable* pPlaceable, CResGFF * pRes, CResStruct * cPlaceableStruct, BOOL bLoadOID, CExoString * pTag);

static Hooks::Hook s_PlaceableLoadFromTemplateHook = nullptr;
static Hooks::Hook s_LoadPlaceableHook = nullptr;

void HardnessToLocal() __attribute__((constructor));
void HardnessToLocal()
{
    s_PlaceableLoadFromTemplateHook = Hooks::HookFunction(&CNWSPlaceable::LoadFromTemplate, &PlaceableLoadFromTemplateHook, Hooks::Order::Late);
    s_LoadPlaceableHook = Hooks::HookFunction(&CNWSPlaceable::LoadPlaceable, &LoadPlaceableHook, Hooks::Order::Late);
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