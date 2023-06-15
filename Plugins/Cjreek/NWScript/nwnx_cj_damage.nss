#include "nwnx"

const string NWNX_Cjreek_Damage = "NWNX_Cjreek"; ///< @private

/// @name Critical Hit Modes
/// @anchor crit_modes
///
/// Used with NWNX_Damage_DealMeleeDamage()
/// @{
const int NWNX_DAMAGE_CRIT_MODE_DONT = 0;
const int NWNX_DAMAGE_CRIT_MODE_ROLL = 1;
const int NWNX_DAMAGE_CRIT_MODE_CRIT = 2;
/// @}

/// @name Sneak Modes
/// @anchor sneak_modes
///
/// Used with NWNX_Damage_DealMeleeDamage()
/// @{
const int NWNX_DAMAGE_SNEAK_MODE_DONT = 0;
const int NWNX_DAMAGE_SNEAK_MODE_CHECK = 1;
const int NWNX_DAMAGE_SNEAK_MODE_DO = 2;
/// @}

/// @brief Deals melee damage to a target
int NWNX_Damage_DealMeleeDamage(object oTarget, object oAttacker = OBJECT_SELF, float fDamageMultiplier = 1.0f, int bOffhand = FALSE, int nCritMode = NWNX_DAMAGE_CRIT_MODE_DONT, int nSneakAttackMode = NWNX_DAMAGE_SNEAK_MODE_DONT, int bDoOnHitEffects = FALSE, int bRollMaxDamage = FALSE, int nMaxOnHitSpellTriggers = 1, int bSkipDevastatingCritEffect = FALSE);

int NWNX_Damage_DealMeleeDamage(object oTarget, object oAttacker = OBJECT_SELF, float fDamageMultiplier = 1.0f, int bOffhand = FALSE, int nCritMode = NWNX_DAMAGE_CRIT_MODE_DONT, int nSneakAttackMode = NWNX_DAMAGE_SNEAK_MODE_DONT, int bDoOnHitEffects = FALSE, int bRollMaxDamage = FALSE, int nMaxOnHitSpellTriggers = 1, int bSkipDevastatingCritEffect = FALSE)
{
    string sFunc = "DealMeleeDamage";

    NWNX_PushArgumentInt(bSkipDevastatingCritEffect);
    NWNX_PushArgumentInt(nMaxOnHitSpellTriggers);
    NWNX_PushArgumentInt(bRollMaxDamage);
    NWNX_PushArgumentInt(bDoOnHitEffects);
    NWNX_PushArgumentInt(nSneakAttackMode);
    NWNX_PushArgumentInt(nCritMode);
    NWNX_PushArgumentInt(bOffhand);
    NWNX_PushArgumentFloat(fDamageMultiplier);
    NWNX_PushArgumentObject(oAttacker);
    NWNX_PushArgumentObject(oTarget);

    NWNX_CallFunction(NWNX_Cjreek_Damage, sFunc);

    return NWNX_GetReturnValueInt();
}
