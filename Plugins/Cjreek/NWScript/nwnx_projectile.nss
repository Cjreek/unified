#include "nwnx"

const string NWNX_Cjreek_Projectile = "NWNX_Cjreek"; ///< @private

const int NWNX_PROJECTILE_SAVING_THROW_NONE = 4;
const int NWNX_PROJECTILE_SAVING_THROW_PARENT = 5;

const int NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO = -1;

const int NWNX_PROJECTILE_SAVING_THROW_DC_AUTO = -1;

const int NWNX_PROJECTILE_DAMAGE_ORIGIN_IMPACT = 0;
const int NWNX_PROJECTILE_DAMAGE_ORIGIN_CASTER = 1;

struct NWNX_Projectile_Projectile
{
    float fSpeed;
    int nProjectileSpellID;
    int nTrailVFX;
    float fHitRadius;
    int bCanHurtCaster;
    int bFriendlyFire;
    int nTargetObjectTypes;
    int bBlockedByTerrain;
    int nPierceCount;
    int bSplashCenteredOnTarget;
    int bAttenuatingSplashDamage;
    int nAttenuatingSplashDamageOrigin;
    float fAttenuatingSplashDamageTarget;
    int bIgnoreSpellResistance;
    int bIsWall;
    int nMaxHitsPerTarget;
    int bHoming;
};

struct NWNX_Projectile_Projectile NWNX_Projectile_GetDefaultProjectile()
{
    struct NWNX_Projectile_Projectile result;
    result.fSpeed = 25.0f;
    result.nProjectileSpellID = 0;
    result.nTrailVFX = 0;
    result.fHitRadius = 0.1f;
    result.bCanHurtCaster = FALSE;
    result.bFriendlyFire = FALSE;
    result.nTargetObjectTypes = OBJECT_TYPE_CREATURE | OBJECT_TYPE_PLACEABLE | OBJECT_TYPE_DOOR;
    result.bBlockedByTerrain = TRUE;
    result.nPierceCount = 0;
    result.bSplashCenteredOnTarget = FALSE;
    result.bAttenuatingSplashDamage = FALSE;
    result.nAttenuatingSplashDamageOrigin = NWNX_PROJECTILE_DAMAGE_ORIGIN_IMPACT;
    result.fAttenuatingSplashDamageTarget = 0.0f;
    result.bIgnoreSpellResistance = FALSE;
    result.bIsWall = FALSE;
    result.nMaxHitsPerTarget = 1;
    result.bHoming = FALSE;

    return result;
}

string NWNX_Projectile_CreateProjectile(struct NWNX_Projectile_Projectile projectileData)
{
    string sFunc = "CreateProjectile";

    NWNX_PushArgumentInt(projectileData.bHoming);
    NWNX_PushArgumentInt(projectileData.nMaxHitsPerTarget);
    NWNX_PushArgumentInt(projectileData.bIsWall);
    NWNX_PushArgumentInt(projectileData.bIgnoreSpellResistance);
    NWNX_PushArgumentFloat(projectileData.fAttenuatingSplashDamageTarget);
    NWNX_PushArgumentInt(projectileData.nAttenuatingSplashDamageOrigin);
    NWNX_PushArgumentInt(projectileData.bAttenuatingSplashDamage);
    NWNX_PushArgumentInt(projectileData.bSplashCenteredOnTarget);
    NWNX_PushArgumentInt(projectileData.nPierceCount);
    NWNX_PushArgumentInt(projectileData.bBlockedByTerrain);
    NWNX_PushArgumentInt(projectileData.nTargetObjectTypes);
    NWNX_PushArgumentInt(projectileData.bFriendlyFire);
    NWNX_PushArgumentInt(projectileData.bCanHurtCaster);
    NWNX_PushArgumentFloat(projectileData.fHitRadius);
    NWNX_PushArgumentInt(projectileData.nTrailVFX);
    NWNX_PushArgumentInt(projectileData.nProjectileSpellID);
    NWNX_PushArgumentFloat(projectileData.fSpeed);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);

    return NWNX_GetReturnValueString();
}

void NWNX_Projectile_AddHitDamage(string sProjectileID, int nHitEffectGroup, int nDamageType, int nNumDice, int nDiceType, float fSplashRadius = 0.0f, int nSavingThrow = NWNX_PROJECTILE_SAVING_THROW_NONE, int nSavingThrowType = NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO, int nSavingThrowDC = NWNX_PROJECTILE_SAVING_THROW_DC_AUTO)
{
    string sFunc = "AddHitEffect";

    effect eEmpty;

    NWNX_PushArgumentInt(-1); // VFX
    NWNX_PushArgumentFloat(fSplashRadius);
    NWNX_PushArgumentInt(nSavingThrowDC);
    NWNX_PushArgumentInt(nSavingThrowType);
    NWNX_PushArgumentInt(nSavingThrow);
    NWNX_PushArgumentString(""); // Hit Script
    NWNX_PushArgumentEffect(eEmpty);
    NWNX_PushArgumentInt(-1); // Max Damage
    NWNX_PushArgumentInt(-1); // Min Damage
    NWNX_PushArgumentInt(nDiceType);
    NWNX_PushArgumentInt(nNumDice);
    NWNX_PushArgumentInt(0); // Damage roll type = dice
    NWNX_PushArgumentInt(nDamageType);
    NWNX_PushArgumentInt(1); // Hit effect type = damage
    NWNX_PushArgumentInt(nHitEffectGroup);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}

void NWNX_Projectile_AddHitDamageRange(string sProjectileID, int nHitEffectGroup, int nDamageType, int nMinDamage, int nMaxDamage, float fSplashRadius = 0.0f, int nSavingThrow = NWNX_PROJECTILE_SAVING_THROW_NONE, int nSavingThrowType = NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO, int nSavingThrowDC = NWNX_PROJECTILE_SAVING_THROW_DC_AUTO)
{
    string sFunc = "AddHitEffect";

    effect eEmpty;
    NWNX_PushArgumentInt(-1); // VFX
    NWNX_PushArgumentFloat(fSplashRadius);
    NWNX_PushArgumentInt(nSavingThrowDC);
    NWNX_PushArgumentInt(nSavingThrowType);
    NWNX_PushArgumentInt(nSavingThrow);
    NWNX_PushArgumentString(""); // Hit Script
    NWNX_PushArgumentEffect(eEmpty);
    NWNX_PushArgumentInt(nMaxDamage); // Max Damage
    NWNX_PushArgumentInt(nMinDamage); // Min Damage
    NWNX_PushArgumentInt(-1); // Dice Type
    NWNX_PushArgumentInt(-1); // Num Dice
    NWNX_PushArgumentInt(1); // Damage roll type = range
    NWNX_PushArgumentInt(nDamageType);
    NWNX_PushArgumentInt(1); // Hit effect type = damage
    NWNX_PushArgumentInt(nHitEffectGroup);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}

void NWNX_Projectile_AddHitEffect(string sProjectileID, int nHitEffectGroup, effect eHitEffect, float fSplashRadius = 0.0f, int nSavingThrow = NWNX_PROJECTILE_SAVING_THROW_NONE, int nSavingThrowType = NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO, int nSavingThrowDC = NWNX_PROJECTILE_SAVING_THROW_DC_AUTO)
{
    string sFunc = "AddHitEffect";

    effect eEmpty;
    NWNX_PushArgumentInt(-1); // VFX
    NWNX_PushArgumentFloat(fSplashRadius);
    NWNX_PushArgumentInt(nSavingThrowDC);
    NWNX_PushArgumentInt(nSavingThrowType);
    NWNX_PushArgumentInt(nSavingThrow);
    NWNX_PushArgumentString(""); // Hit Script
    NWNX_PushArgumentEffect(eHitEffect);
    NWNX_PushArgumentInt(-1); // Max Damage
    NWNX_PushArgumentInt(-1); // Min Damage
    NWNX_PushArgumentInt(-1); // Dice Type
    NWNX_PushArgumentInt(-1); // Num Dice
    NWNX_PushArgumentInt(-1); // Damage roll type = range
    NWNX_PushArgumentInt(-1); // Damage Type
    NWNX_PushArgumentInt(2); // Hit effect type = effect
    NWNX_PushArgumentInt(nHitEffectGroup);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}

void NWNX_Projectile_AddHitScript(string sProjectileID, int nHitEffectGroup, string sHitEffectScript, float fSplashRadius = 0.0f, int nSavingThrow = NWNX_PROJECTILE_SAVING_THROW_NONE, int nSavingThrowType = NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO, int nSavingThrowDC = NWNX_PROJECTILE_SAVING_THROW_DC_AUTO)
{
    string sFunc = "AddHitEffect";

    effect eEmpty;
    NWNX_PushArgumentInt(-1); // VFX
    NWNX_PushArgumentFloat(fSplashRadius);
    NWNX_PushArgumentInt(nSavingThrowDC);
    NWNX_PushArgumentInt(nSavingThrowType);
    NWNX_PushArgumentInt(nSavingThrow);
    NWNX_PushArgumentString(sHitEffectScript);
    NWNX_PushArgumentEffect(eEmpty);
    NWNX_PushArgumentInt(-1); // Max Damage
    NWNX_PushArgumentInt(-1); // Min Damage
    NWNX_PushArgumentInt(-1); // Dice Type
    NWNX_PushArgumentInt(-1); // Num Dice
    NWNX_PushArgumentInt(-1); // Damage roll type = range
    NWNX_PushArgumentInt(-1); // Damage Type
    NWNX_PushArgumentInt(3); // Hit effect type = script
    NWNX_PushArgumentInt(nHitEffectGroup);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}

void NWNX_Projectile_AddHitVFX(string sProjectileID, int nHitEffectGroup, int nVFX, float fSplashRadius = 0.0f)
{
    string sFunc = "AddHitEffect";

    effect eEmpty;
    NWNX_PushArgumentInt(nVFX);
    NWNX_PushArgumentFloat(fSplashRadius);
    NWNX_PushArgumentInt(NWNX_PROJECTILE_SAVING_THROW_DC_AUTO);
    NWNX_PushArgumentInt(NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO);
    NWNX_PushArgumentInt(NWNX_PROJECTILE_SAVING_THROW_NONE);
    NWNX_PushArgumentString(""); // Hit Script
    NWNX_PushArgumentEffect(eEmpty);
    NWNX_PushArgumentInt(-1); // Max Damage
    NWNX_PushArgumentInt(-1); // Min Damage
    NWNX_PushArgumentInt(-1); // Dice Type
    NWNX_PushArgumentInt(-1); // Num Dice
    NWNX_PushArgumentInt(-1); // Damage roll type = range
    NWNX_PushArgumentInt(-1); // Damage Type
    NWNX_PushArgumentInt(4); // Hit effect type = VFX
    NWNX_PushArgumentInt(nHitEffectGroup);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}

void NWNX_Projectile_SetHitGroupSavingThrow(string sProjectileID, int nHitEffectGroup, int nSavingThrow, int nSavingThrowType = NWNX_PROJECTILE_SAVING_THROW_TYPE_AUTO, int nSavingThrowDC = NWNX_PROJECTILE_SAVING_THROW_DC_AUTO)
{
    string sFunc = "SetHitGroupSavingThrow";

    NWNX_PushArgumentInt(nSavingThrowDC);
    NWNX_PushArgumentInt(nSavingThrowType);
    NWNX_PushArgumentInt(nSavingThrow);
    NWNX_PushArgumentInt(nHitEffectGroup);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}


void NWNX_Projectile_FireProjectile(string sProjectileID, vector vOrigin, vector vEnd, object oSpellTarget, float fProjectileSpeed = 0.0f, object oCaster = OBJECT_SELF)
{
    string sFunc = "FireProjectile";

    NWNX_PushArgumentObject(oCaster);
    NWNX_PushArgumentFloat(fProjectileSpeed);
    NWNX_PushArgumentObject(oSpellTarget);
    NWNX_PushArgumentFloat(vEnd.z);
    NWNX_PushArgumentFloat(vEnd.y);
    NWNX_PushArgumentFloat(vEnd.x);
    NWNX_PushArgumentFloat(vOrigin.z);
    NWNX_PushArgumentFloat(vOrigin.y);
    NWNX_PushArgumentFloat(vOrigin.x);
    NWNX_PushArgumentString(sProjectileID);

    NWNX_CallFunction(NWNX_Cjreek_Projectile, sFunc);
}
