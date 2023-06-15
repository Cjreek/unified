#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

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
