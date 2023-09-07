#include "nwnx.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CWorldTimer.hpp"
#include "API/CNWSObject.hpp"
#include "API/CNWSObjectActionNode.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

static int16_t GetMaxHitPointsHook(CNWSCreature* pCreature, BOOL bIncludeToughness = true);
static uint32_t AIActionCastSpellHook(CNWSCreature* pCreature, CNWSObjectActionNode * pNode);

static Hooks::Hook s_GetMaxHitPointsHook = nullptr;
static Hooks::Hook s_AIActionCastSpellHook = nullptr;

void CreatureExtension() __attribute__((constructor));
void CreatureExtension()
{
    s_GetMaxHitPointsHook = Hooks::HookFunction(Functions::_ZN12CNWSCreature15GetMaxHitPointsEi, &GetMaxHitPointsHook, Hooks::Order::Late);
    s_AIActionCastSpellHook = Hooks::HookFunction(&CNWSCreature::AIActionCastSpell, &AIActionCastSpellHook, Hooks::Order::Late);
}

static uint32_t AIActionCastSpellHook(CNWSCreature* pCreature, CNWSObjectActionNode* pNode)
{
    uint32_t nResult = 0;

    CNWSObjectActionNode* pChannelNode = nullptr;
    if (auto pChannelNodeOptional = pCreature->nwnxGet<void*>("SPELL_CHANNEL_ACTION_NODE"))
        pChannelNode = (CNWSObjectActionNode*)pChannelNodeOptional.value();

    if (pNode == pChannelNode)
    {
        auto pQueuedActionList = pCreature->m_lQueuedActions.m_pcExoLinkedListInternal;
        for (auto *pNode = pQueuedActionList->pHead; pNode; pNode = pNode->pNext)
        {
            CNWSObjectActionNode* pAction = static_cast<CNWSObjectActionNode*>(pNode->pObject);
            if ((pAction->m_nActionId == 1) || (pAction->m_nActionId == 51))
            {
                nResult = 2;
                break;
            }
        }

        if (nResult != 2)
        {
            uint32_t nStartDay = 0;
            if (auto nStartDayOptional = pCreature->nwnxGet<int32_t>("SPELL_CHANNEL_START_DAY"))
                nStartDay = *(uint32_t*)&(nStartDayOptional.value());

            uint32_t nStartTime = 0;
            if (auto nStartTimeOptional = pCreature->nwnxGet<int32_t>("SPELL_CHANNEL_START_TIME"))
                nStartTime = *(uint32_t*)&(nStartTimeOptional.value());

            uint64_t nDuration = 0;
            if (auto nDurationOptional = pCreature->nwnxGet<int32_t>("SPELL_CHANNEL_DURATION"))
                nDuration = nDurationOptional.value();

            uint64_t nDifference = Globals::AppManager()->m_pServerExoApp->GetWorldTimer()->GetTimeDifferenceFromWorldTime(nStartDay, nStartTime);
            if (nDifference < nDuration)
                nResult = 1;
            else
                nResult = 2;
        }

        if (nResult == 2)
        {
            pCreature->nwnxRemove("SPELL_CHANNEL_ACTION_NODE");
            pCreature->nwnxRemove("SPELL_CHANNEL_START_DAY");
            pCreature->nwnxRemove("SPELL_CHANNEL_START_TIME");
            pCreature->nwnxRemove("SPELL_CHANNEL_DURATION");
        }
    }
    else
    {
        nResult = s_AIActionCastSpellHook->CallOriginal<uint32_t>(pCreature, pNode);
        if (nResult == 2)
        {
            pCreature->nwnxSet("SPELL_CHANNEL_ACTION_NODE", (void*)pNode, nullptr);

            uint32_t nDay;
            uint32_t nTime;
            Globals::AppManager()->m_pServerExoApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
            pCreature->nwnxSet("SPELL_CHANNEL_START_DAY", *(int32_t*)&nDay, false);
            pCreature->nwnxSet("SPELL_CHANNEL_START_TIME", *(int32_t*)&nTime, false);

            nResult = 1;
        }
    }

    return nResult;
}

static int16_t GetMaxHitPointsHook(CNWSCreature* pCreature, BOOL bIncludeToughness)
{
    int16_t nBaseHitPoints = s_GetMaxHitPointsHook->CallOriginal<int16_t>(pCreature, bIncludeToughness);

    int32_t nBonusHitPoints = 0;
    if (auto hitpointBonusOptional = pCreature->nwnxGet<int32_t>("HITPOINT_BONUS"))
        nBonusHitPoints = hitpointBonusOptional.value();

    int32_t nHitPointIncrease = 0;
    if (auto hitpointIncreaseOptional = pCreature->nwnxGet<int32_t>("HITPOINT_INCREASE"))
        nHitPointIncrease = hitpointIncreaseOptional.value();

    return (nBaseHitPoints + nBonusHitPoints) * (1.0f + (nHitPointIncrease / 100.0f));
}

NWNX_EXPORT ArgumentStack RestoreSpells(ArgumentStack&& args)
{
    if (auto *pCreature = Utils::PopCreature(args))
    {
        const auto level = args.extract<int32_t>();
          ASSERT_OR_THROW(level >= -1);
          ASSERT_OR_THROW(level <= 9);

        if (level >= 0 && level <= 9)
        {
            pCreature->m_pStats->ReadySpellLevel(level);
        }
        else
        {
            for (int i = 0; i <= 9; i++)
               pCreature->m_pStats->ReadySpellLevel(i);
        }
    }
    return {};
}

NWNX_EXPORT ArgumentStack GetMaximumKnownSpellLevel(ArgumentStack&& args)
{
    if (auto *pCreature = Utils::PopCreature(args))
    {
        auto nClass = args.extract<int32_t>();
        ASSERT_OR_THROW(nClass >= Constants::ClassType::MIN);
        ASSERT_OR_THROW(nClass <= Constants::ClassType::MAX);

        for (int i=0; i < pCreature->m_pStats->m_nNumMultiClasses; i++)
        {
            if (pCreature->m_pStats->GetClass(i) == nClass)
            {
                int nSpellLevel = 0;
                while (pCreature->m_pStats->GetSpellGainWithBonus(i, nSpellLevel) > 0)
                    nSpellLevel++;

                return nSpellLevel - 1;
            }
        }
    }

    return -1;
}

NWNX_EXPORT ArgumentStack UpdateCombatInformation(ArgumentStack&& args)
{
    if (auto *pCreature = Utils::PopCreature(args))
    {
        pCreature->m_pStats->UpdateCombatInformation();
    }

    return {};
}

NWNX_EXPORT ArgumentStack SetAttackSpeed(ArgumentStack&& args)
{
    if (auto* pCreature = Utils::PopCreature(args))
    {
        auto fAttackSpeed = args.extract<float>();
        auto fAttackDelay = 1.0f / fAttackSpeed;

        pCreature->nwnxSet("ATTACK_DELAY", fAttackDelay, false);
    }

    return {};
}

NWNX_EXPORT ArgumentStack SetHitPointBonus(ArgumentStack&& args)
{
    if (auto* pCreature = Utils::PopCreature(args))
    {
        auto nHitPointBonus = args.extract<int32_t>();
        pCreature->nwnxSet("HITPOINT_BONUS", nHitPointBonus, false);
    }

    return {};
}

NWNX_EXPORT ArgumentStack SetHitPointIncrease(ArgumentStack&& args)
{
    if (auto* pCreature = Utils::PopCreature(args))
    {
        auto nHitPointIncrease = args.extract<int32_t>();
        pCreature->nwnxSet("HITPOINT_INCREASE", nHitPointIncrease, false);
    }

    return {};
}

NWNX_EXPORT ArgumentStack SetSpellChannelDuration(ArgumentStack&& args)
{
    if (auto* pCreature = Utils::PopCreature(args))
    {
        auto fDuration = args.extract<float>();
        pCreature->nwnxSet("SPELL_CHANNEL_DURATION", (int32_t)(fDuration * 1000.0f), false);
    }

    return {};
}
