#include "nwnx.hpp"
#include "API/CNWSUUID.hpp"
#include "API/CGameEffect.hpp"
#include "API/CNWSObject.hpp"
#include "API/CNWSCreature.hpp"
#include "API/CNWSArea.hpp"
#include "API/CNWSWaypoint.hpp"
#include "API/CNWSPlaceable.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWSModule.hpp"
#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerExoAppInternal.hpp"
#include "API/CServerAIMaster.hpp"
#include "API/CWorldTimer.hpp"

#include <functional>
#include <queue>

using namespace NWNXLib;
using namespace NWNXLib::API;

struct HitEffect
{
    int nHitEffectType; // 1 = Damage, 2 = Effect, 3 = Script, 4 = VFX

    // Type 1: Damage
    int nDamageType;
    int nDamageRollType; // 0 = dice, 1 = range
    int nDiceNum;
    int nDiceType;
    int nDamageMin;
    int nDamageMax;
    // Type 2: Effect
    CGameEffect* pEffect;
    // Type 3: Script
    std::string sHitScript;

    int nSavingThrow;
    int nSavingThrowType; // -1 Auto
    int nDC; // -1 = Auto

    float fSplashRadius;
    int nImpactVFX;
};

struct HitEffectGroup
{
    int nSavingThrow = -1;
    int nSavingThrowType = -1; // -1 Auto
    int nDC = -1; // -1 = Auto

    std::vector<HitEffect> hitEffects;
};

struct Projectile
{
    std::string sID;
    float fSpeed;
    int nProjectileSpellID;
    int nTrailVFX;
    float fHitRadius;
    bool bCanHurtCaster;
    bool bFriendlyFire;
    int nTargetObjectTypes;
    bool bBlockedByTerrain;
    int nPierceCount;
    bool bSplashCenteredOnTarget;
    bool bAttenuatingSplashDamage;
    int nAttenuatingSplashDamageOrigin;
    float fAttenuatingSplashDamageTarget;
    bool bIgnoreSpellResistance;
    bool bIsWall;
    int nMaxHitsPerTarget;
    bool bHoming;

    HitEffectGroup hitGroups[10];
};

struct Shot
{
    std::string sID;
    std::string sProjectileID;
    Vector vOrigin;
    Vector vDestination;
    OBJECT_ID oidCaster;
    OBJECT_ID oidTarget;
    uint32_t nFlightTimeMs;

    OBJECT_ID oidFakeTarget;
    std::chrono::steady_clock::time_point StartTime;
    Vector vCurrentPosition;
};

constexpr uint32_t EVENT_DELAY_COMMAND = 2345;
constexpr uint32_t FAKE_TARGET_CACHE_SIZE = 50;

static std::unordered_map<std::string, Projectile> s_Projectiles;
static std::unordered_map<std::string, Shot> s_ProjectileShots;
static std::unordered_map<OBJECT_ID, std::queue<OBJECT_ID>> s_FakeTargetCache;

typedef void(*CommandFunc)(void);

struct CommandFuncAsPtr {
    CommandFuncAsPtr(std::function<void()> cf) : f(cf) {}
    std::function<void()> f;
    void operator()() { f(); }
};

static void ModuleEventHandlerHook(CNWSModule*, uint32_t, ObjectID, void*, uint32_t, uint32_t);
static BOOL LoadPlaceablesHook(CNWSArea* pArea, CResGFF * pRes, CResStruct * pStruct, BOOL bLoadStateInfo, BOOL bLoadOID);

static Hooks::Hook s_ModuleEventHandlerHook = nullptr;
static Hooks::Hook s_LoadPlaceablesHook = nullptr;

void ProjectilePlugin() __attribute__((constructor));
void ProjectilePlugin()
{
    s_ModuleEventHandlerHook = Hooks::HookFunction(&CNWSModule::EventHandler, &ModuleEventHandlerHook, Hooks::Order::Late);
    s_LoadPlaceablesHook = Hooks::HookFunction(&CNWSArea::LoadPlaceables, &LoadPlaceablesHook, Hooks::Order::Late);
}

static CNWSPlaceable* GetFakeTarget(CNWSArea* pArea)
{
    if (s_FakeTargetCache.find(pArea->m_idSelf) == s_FakeTargetCache.end())
        return nullptr;

    std::queue<OBJECT_ID>& fakeTargetAreaCache = s_FakeTargetCache[pArea->m_idSelf];

    auto oidResult = fakeTargetAreaCache.front();
    fakeTargetAreaCache.pop();
    
    return Utils::AsNWSPlaceable(Utils::GetGameObject(oidResult));
}

static BOOL LoadPlaceablesHook(CNWSArea* pArea, CResGFF* pRes, CResStruct * pStruct, BOOL bLoadStateInfo, BOOL bLoadOID)
{
    auto result = s_LoadPlaceablesHook->CallOriginal<BOOL>(pArea, pRes, pStruct, bLoadStateInfo, bLoadOID);

    if (s_FakeTargetCache.find(pArea->m_idSelf) == s_FakeTargetCache.end())
    {
        std::queue<OBJECT_ID> newCache;
        s_FakeTargetCache[pArea->m_idSelf] = newCache;
    }
    
    std::queue<OBJECT_ID>& areaCache = s_FakeTargetCache[pArea->m_idSelf];
    
    for (uint32_t i=0; i < FAKE_TARGET_CACHE_SIZE; i++)
    {
        auto pFakeTarget = new CNWSPlaceable();
        pFakeTarget->LoadFromTemplate(CResRef("plc_invisobj"));
        pFakeTarget->SetPosition(Vector());
        pFakeTarget->m_fVisibleDistance = 999999.0f;
        pFakeTarget->AddToArea(pArea, 0, 0, 0, false);

        areaCache.push(pFakeTarget->m_idSelf);
    }

    return result;
}

static void ModuleEventHandlerHook(CNWSModule *pModule, uint32_t nEventId, ObjectID nCallerObjectId, void *pEventData, uint32_t nCalendarDay, uint32_t nTimeOfDay)
{
    if (nEventId == EVENT_DELAY_COMMAND)
    {
        auto cmd = static_cast<CommandFuncAsPtr*>(pEventData);
        (*cmd)();
        delete cmd;
    }
    else
    {
        s_ModuleEventHandlerHook->CallOriginal<void>(pModule, nEventId, nCallerObjectId, pEventData, nCalendarDay, nTimeOfDay);
    }
}

void DelayCommand(float fDelay, const std::function<void()> &command)
{
    uint32_t days = Globals::AppManager()->m_pServerExoApp->GetWorldTimer()->GetCalendarDayFromSeconds(fDelay);
    uint32_t time = Globals::AppManager()->m_pServerExoApp->GetWorldTimer()->GetTimeOfDayFromSeconds(fDelay);

    Globals::AppManager()->m_pServerExoApp->GetServerAIMaster()->AddEventDeltaTime(
                days, time, Utils::GetModule()->m_idSelf,
                Utils::GetModule()->m_idSelf, EVENT_DELAY_COMMAND, new CommandFuncAsPtr(command));
}

NWNX_EXPORT ArgumentStack CreateProjectile(ArgumentStack&& args)
{
    ArgumentStack stack;

    Projectile newProjectile;
    newProjectile.sID = CNWSUUID::MakeRandom();
    newProjectile.fSpeed = args.extract<float>();
    newProjectile.nProjectileSpellID = args.extract<int32_t>();
    newProjectile.nTrailVFX = args.extract<int32_t>();
    newProjectile.fHitRadius = args.extract<float>();
    newProjectile.bCanHurtCaster = args.extract<int32_t>() != 0;
    newProjectile.bFriendlyFire = args.extract<int32_t>() != 0;
    newProjectile.nTargetObjectTypes = args.extract<int32_t>();
    newProjectile.bBlockedByTerrain = args.extract<int32_t>() != 0;
    newProjectile.nPierceCount = args.extract<int32_t>();
    newProjectile.bSplashCenteredOnTarget = args.extract<int32_t>() != 0;
    newProjectile.bAttenuatingSplashDamage = args.extract<int32_t>() != 0;
    newProjectile.nAttenuatingSplashDamageOrigin = args.extract<int32_t>();
    newProjectile.fAttenuatingSplashDamageTarget = args.extract<float>();
    newProjectile.bIgnoreSpellResistance = args.extract<int32_t>() != 0;
    newProjectile.bIsWall = args.extract<int32_t>() != 0;
    newProjectile.nMaxHitsPerTarget = args.extract<int32_t>();
    newProjectile.bHoming = args.extract<int32_t>() != 0;

    s_Projectiles[newProjectile.sID] = newProjectile;

    return newProjectile.sID;
}

NWNX_EXPORT ArgumentStack AddHitEffect(ArgumentStack&& args)
{
    std::string projectileID = args.extract<std::string>();
        ASSERT(s_Projectiles.find(projectileID) != s_Projectiles.end());
    int32_t hitGroup = args.extract<int32_t>();
        ASSERT((hitGroup >= 1) && (hitGroup <= 10));

    HitEffect hitEffect;
    hitEffect.nHitEffectType = args.extract<int32_t>();
    hitEffect.nDamageType = args.extract<int32_t>();
    hitEffect.nDamageRollType = args.extract<int32_t>();
    hitEffect.nDiceNum = args.extract<int32_t>();
    hitEffect.nDiceType = args.extract<int32_t>();
    hitEffect.nDamageMin = args.extract<int32_t>();
    hitEffect.nDamageMax = args.extract<int32_t>();
    hitEffect.pEffect = args.extract<CGameEffect*>();
    hitEffect.sHitScript = args.extract<std::string>();
    hitEffect.nSavingThrow = args.extract<int32_t>();
    hitEffect.nSavingThrowType = args.extract<int32_t>();
    hitEffect.nDC = args.extract<int32_t>();
    hitEffect.fSplashRadius = args.extract<float>();
    hitEffect.nImpactVFX = args.extract<int32_t>();

    s_Projectiles[projectileID].hitGroups[hitGroup-1].hitEffects.push_back(hitEffect);

    return {};
}

NWNX_EXPORT ArgumentStack SetHitGroupSavingThrow(ArgumentStack&& args)
{
    std::string projectileID = args.extract<std::string>();
        ASSERT(s_Projectiles.find(projectileID) != s_Projectiles.end());
    int32_t hitGroupNumber = args.extract<int32_t>();
        ASSERT((hitGroupNumber >= 1) && (hitGroupNumber <= 10));

    auto& hitGroup = s_Projectiles[projectileID].hitGroups[hitGroupNumber-1];
    hitGroup.nSavingThrow = args.extract<int32_t>();
    hitGroup.nSavingThrowType = args.extract<int32_t>();
    hitGroup.nDC = args.extract<int32_t>();

    return {};
}

NWNX_EXPORT ArgumentStack FireProjectile(ArgumentStack&& args)
{
    std::string projectileID = args.extract<std::string>();
        ASSERT(s_Projectiles.find(projectileID) != s_Projectiles.end());

    auto& pProjectile = s_Projectiles[projectileID];

    Shot shot;
    shot.sID = CNWSUUID::MakeRandom();
    shot.sProjectileID = projectileID;

    shot.vOrigin.x = args.extract<float>();
    shot.vOrigin.y = args.extract<float>();
    shot.vOrigin.z = args.extract<float>();

    shot.vDestination.x = args.extract<float>();
    shot.vDestination.y = args.extract<float>();
    shot.vDestination.z = args.extract<float>();

    auto pTarget = Utils::PopObject(args, false);
    float fSpeed = args.extract<float>();
    auto pCaster = Utils::PopCreature(args);

    shot.oidCaster = pCaster->m_idSelf;
    if (pTarget)
        shot.oidTarget = pTarget->m_idSelf;

    if (fSpeed == 0.0f) 
        fSpeed = pProjectile.fSpeed;

    if ((pTarget) && (Vector::MagnitudeSquared(shot.vDestination) == 0.0f))
        shot.vDestination = pTarget->m_vPosition;
        
    if (Vector::MagnitudeSquared(shot.vOrigin) == 0.0f)
        shot.vOrigin = pCaster->m_vPosition;

    shot.nFlightTimeMs = (uint32_t)((Vector::Magnitude(shot.vDestination - shot.vOrigin) / fSpeed) * 1000);

    // pCaster->GetArea()->ComputeHeight(shot.vDestination)

    auto pFakeObject = GetFakeTarget(pCaster->GetArea());
    pFakeObject->SetPosition(shot.vDestination);

    // auto pFakeObject = new CNWSPlaceable();
    // pFakeObject->LoadFromTemplate(CResRef("plc_invisobj"));
    // pFakeObject->SetPosition(shot.vDestination);
    // pFakeObject->AddToArea(pCaster->GetArea(), shot.vDestination.x, shot.vDestination.y, shot.vDestination.z, false);

    shot.oidFakeTarget = pFakeObject->m_idSelf;
    shot.vCurrentPosition = shot.vOrigin;
    shot.StartTime = std::chrono::steady_clock::now();

    s_ProjectileShots[shot.sID] = shot;

    pCaster->BroadcastSafeProjectile(shot.oidCaster, shot.oidFakeTarget, shot.vOrigin, shot.vDestination, shot.nFlightTimeMs, 6, pProjectile.nProjectileSpellID, 1);

    // auto oidCaster = shot.oidCaster;
    // auto shotId = shot.sID;
    // DelayCommand(0.4f, [oidCaster, shotId, projectileID](){
    //     auto pCaster = Utils::GetGameObject(oidCaster)->AsNWSObject();
    //     auto pProjectile = s_Projectiles[projectileID];
    //     auto shot = s_ProjectileShots[shotId];

    //     pCaster->BroadcastSafeProjectile(shot.oidCaster, shot.oidFakeTarget, shot.vOrigin, shot.vDestination, shot.nFlightTimeMs, 6, pProjectile.nProjectileSpellID, 1);
    // });
    
    auto oidFakeTarget = shot.oidFakeTarget;
    auto oidPlayer = pTarget->m_idSelf;
    DelayCommand(1.5f, [oidFakeTarget, oidPlayer](){
        auto* pFakeTarget = Utils::AsNWSPlaceable(Utils::GetGameObject(oidFakeTarget));
        Vector vMove = pFakeTarget->m_vPosition - Vector(5.0, 0.0, 0.0);

        pFakeTarget->SetPosition(vMove);
    });

    return {};
}

// Missing 
/*
int SPLASH_IGNORES_HITERATOR        = 2048;   // Splash damage can damage repeatedly and indiscriminately so long as something procs the projectile.
int EFFECT_ROULETTE                 = 4096;   // !Damage stages spin around, rather than constantly apply the last one.
int ALWAYS_SPLASHES_AT_END          = 131072; // Always throws a final splash when it reaches its destination.
int USE_SECONDARY_ORIGIN            = 524288; // Use the secondary origin point as the actual cast point.  If none is set, or it is in another area, then use caster anyway.
*/