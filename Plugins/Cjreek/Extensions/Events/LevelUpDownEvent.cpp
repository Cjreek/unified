#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/CNWLevelStats.hpp"
#include "API/CNWRules.hpp"
#include "API/CNWClass.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

static void SetExperienceHook(CNWSCreatureStats* pCreatureStats, uint32_t nValue, BOOL bDoLevel = true);
static Hooks::Hook s_SetExperienceHook = nullptr;

void LevelUpDownEvent() __attribute__((constructor));
void LevelUpDownEvent()
{
    s_SetExperienceHook = Hooks::HookFunction(&CNWSCreatureStats::SetExperience, &SetExperienceHook, Hooks::Order::Late);
}

void HandleLevelUpDown(CNWSCreatureStats* pCreatureStats, int nLevelBefore, int nLevelAfter)
{
    if (nLevelAfter > nLevelBefore)
    {
        for (uint8_t i = nLevelBefore; i < nLevelAfter; i++)
        {
            CNWLevelStats* pLevelUpStats = new CNWLevelStats();
            pLevelUpStats->m_bEpic = i > 20;
            pLevelUpStats->m_nClass = pCreatureStats->GetClass(0);
            pLevelUpStats->m_nHitDie = Globals::Rules()->m_lstClasses[pLevelUpStats->m_nClass].m_nHitDie;
            pLevelUpStats->m_nSkillPointsRemaining = 0;
            pLevelUpStats->m_nAbilityGain = 6;
            pCreatureStats->LevelUp(pLevelUpStats, 0xFF, 0xFF, 0xFF, true);
        }

        MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", { "LEVEL_FROM", std::to_string(nLevelBefore) });
        MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", { "LEVEL_TO", std::to_string(nLevelAfter) });
        MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", { "NWNX_ON_PLAYER_LEVELUP", Utils::ObjectIDToString(pCreatureStats->m_pBaseCreature->m_idSelf) });
    }
    else if (nLevelAfter < nLevelBefore)
    {
        MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", { "LEVEL_FROM", std::to_string(nLevelBefore) });
        MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", { "LEVEL_TO", std::to_string(nLevelAfter) });
        MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", { "NWNX_ON_PLAYER_LEVELDOWN", Utils::ObjectIDToString(pCreatureStats->m_pBaseCreature->m_idSelf) });
    }
}

static void SetExperienceHook(CNWSCreatureStats* pCreatureStats, uint32_t nValue, BOOL bDoLevel)
{
    if (pCreatureStats->m_pBaseCreature->m_bPlayerCharacter)
    {
        uint8_t nLevelBefore = pCreatureStats->GetLevel(false);
        s_SetExperienceHook->CallOriginal<void>(pCreatureStats, nValue, bDoLevel);
        uint8_t nPotentialLevel = pCreatureStats->GetPotentialLevel();

        HandleLevelUpDown(pCreatureStats, nLevelBefore, nPotentialLevel);
    }
    else
        s_SetExperienceHook->CallOriginal<void>(pCreatureStats, nValue, bDoLevel);
}
