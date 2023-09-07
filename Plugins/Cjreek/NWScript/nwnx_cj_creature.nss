#include "nwnx"

const string NWNX_Cjreek_Creature = "NWNX_Cjreek"; ///< @private

/// @brief Restore all creature spells per day for given level.
/// @param creature The creature object.
/// @param level The level to restore. If -1, all spells are restored.
void NWNX_Creature_RestoreSpells(object creature, int level = -1);

int NWNX_Creature_GetMaximumKnownSpellLevel(object oCreature, int nClass);
void NWNX_Creature_UpdateCombatInformation(object oCreature);
void NWNX_Creature_SetAttackSpeed(object oCreature, float fAttackSpeed);
void NWNX_Creature_SetHitPointBonus(object oCreature, int nHitPointBonus);
void NWNX_Creature_SetHitPointIncrease(object oCreature, int nHitPointIncrease);
void NWNX_Creature_SetSpellChannelDuration(object oCreature, float fDuration);

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

void NWNX_Creature_SetAttackSpeed(object oCreature, float fAttackSpeed)
{
    string sFunc = "SetAttackSpeed";

    NWNX_PushArgumentFloat(fAttackSpeed);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);
}

void NWNX_Creature_SetHitPointBonus(object oCreature, int nHitPointBonus)
{
    string sFunc = "SetHitPointBonus";

    NWNX_PushArgumentInt(nHitPointBonus);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);
}

void NWNX_Creature_SetHitPointIncrease(object oCreature, int nHitPointIncrease)
{
    string sFunc = "SetHitPointIncrease";

    NWNX_PushArgumentInt(nHitPointIncrease);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);
}

void NWNX_Creature_SetSpellChannelDuration(object oCreature, float fDuration)
{
    string sFunc = "SetSpellChannelDuration";

    NWNX_PushArgumentFloat(fDuration);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Creature, sFunc);
}
