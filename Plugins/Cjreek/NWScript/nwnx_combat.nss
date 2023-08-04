#include "nwnx"

const string NWNX_Cjreek_Combat = "NWNX_Cjreek"; ///< @private

void NWNX_Combat_SetCreatureAttackSpeed(object oCreature, float fAttackSpeed);
void NWNX_Combat_EndCombatRound(object oCreature, int bOnlyIfMeleeRound = TRUE);

void NWNX_Combat_EndCombatRound(object oCreature, int bOnlyIfMeleeRound)
{
    string sFunc = "EndCombatRound";

    NWNX_PushArgumentInt(bOnlyIfMeleeRound);
    NWNX_PushArgumentObject(oCreature);

    NWNX_CallFunction(NWNX_Cjreek_Combat, sFunc);
}
