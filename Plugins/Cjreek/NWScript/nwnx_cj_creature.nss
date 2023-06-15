#include "nwnx"

const string NWNX_Cjreek_Creature = "NWNX_Cjreek"; ///< @private

/// @brief Restore all creature spells per day for given level.
/// @param creature The creature object.
/// @param level The level to restore. If -1, all spells are restored.
void NWNX_Creature_RestoreSpells(object creature, int level = -1);

int NWNX_Creature_GetMaximumKnownSpellLevel(object oCreature, int nClass);
void NWNX_Creature_UpdateCombatInformation(object oCreature);

void NWNX_Creature_RestoreSpells(object creature, int level = -1)
{
    string sFunc = "RestoreSpells";
    NWNX_PushArgumentInt(level);
    NWNX_PushArgumentObject(creature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);
}

int NWNX_Creature_GetMaximumKnownSpellLevel(object oCreature, int nClass)
{
    string sFunc = "GetMaximumKnownSpellLevel";

    NWNX_PushArgumentInt(nClass);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);

    return NWNX_GetReturnValueInt();
}

void NWNX_Creature_UpdateCombatInformation(object oCreature)
{
    string sFunc = "UpdateCombatInformation";
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);
}
