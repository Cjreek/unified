#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

static int16_t GetMaxHitPointsHook(CNWSCreature* pCreature, BOOL bIncludeToughness = true);

static Hooks::Hook s_GetMaxHitPointsHook = nullptr;

void CreatureExtension() __attribute__((constructor));
void CreatureExtension()
{
    s_GetMaxHitPointsHook = Hooks::HookFunction(Functions::_ZN12CNWSCreature15GetMaxHitPointsEi, &GetMaxHitPointsHook, Hooks::Order::Late);
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
