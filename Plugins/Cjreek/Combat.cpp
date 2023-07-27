#include "nwnx.hpp"

#include "API/CNWSCreature.hpp"
#include "API/CNWSCreatureStats.hpp"
#include "API/CNWSCombatRound.hpp"
#include "API/CNWSCombatRoundAction.hpp"
#include "API/CNWSItem.hpp"
#include "API/CNWSModule.hpp"
#include "API/CNWSInventory.hpp"

#include "API/CAppManager.hpp"
#include "API/CServerExoApp.hpp"
#include "API/CServerAIMaster.hpp"
#include "API/CScriptEvent.hpp"
#include "API/CNWSPlayer.hpp"
#include "API/CNWSMessage.hpp"
#include "API/CNWCCMessageData.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

static void InitializeAttackActionsHook(CNWSCombatRound* pCombatRound, OBJECT_ID oidTarget);
static void AddCombatStepActionHook(CNWSCombatRound* pCombatRound, uint32_t nTimeIndex, OBJECT_ID oidTarget);
static void ResolveMeleeAnimationsHook(CNWSCreature* pCreature, int32_t nAttackIndex, int32_t nAttacks, CNWSObject* pTarget, int32_t nTimeAnimation);

static Hooks::Hook s_InitializeAttackActionsHook = nullptr;
static Hooks::Hook s_AddCombatStepActionHook = nullptr;
static Hooks::Hook s_ResolveMeleeAnimationsHook = nullptr;

void Combat() __attribute__((constructor));
void Combat()
{
    s_InitializeAttackActionsHook = Hooks::HookFunction(&CNWSCombatRound::InitializeAttackActions, &InitializeAttackActionsHook, Hooks::Order::Final);
    s_AddCombatStepActionHook = Hooks::HookFunction(&CNWSCombatRound::AddCombatStepAction, &AddCombatStepActionHook, Hooks::Order::Late);
    s_ResolveMeleeAnimationsHook = Hooks::HookFunction(&CNWSCreature::ResolveMeleeAnimations, &ResolveMeleeAnimationsHook, Hooks::Order::Final);
}

void SendFloatyMessage(CNWSCreature* pCreature, std::string message)
{
    if (pCreature->m_bPlayerCharacter)
    {
        auto* pPlayer = Globals::AppManager()->m_pServerExoApp->GetClientObjectByObjectId(pCreature->m_idSelf);

        CNWCCMessageData* msgData = new CNWCCMessageData();
        msgData->SetObjectID(0, pCreature->m_idSelf);
        msgData->SetString(0, message);
        msgData->SetInteger(9, 94);

        CNWSMessage* pMessage = Globals::AppManager()->m_pServerExoApp->GetNWSMessage();
        pMessage->SendServerToPlayerCCMessage(pPlayer->m_nPlayerID, 11, msgData, nullptr);
    }
}

static void ResolveMeleeAnimationsHook(CNWSCreature* pCreature, int32_t nAttackIndex, int32_t nAttacks, CNWSObject* pTarget, int32_t nTimeAnimation)
{
    if (!pTarget) return;
  
    auto* pAttack = pCreature->m_pcCombatRound->GetAttack(pCreature->m_pcCombatRound->m_nCurrentAttack); // nAttackIndex?
    pAttack->m_nAnimationLength = nTimeAnimation;
    pAttack->m_nReactionAnimation = 1;
    pAttack->m_nReactionAnimationLength = nTimeAnimation / 2;

    auto nCurrentAction = pTarget->m_nCurrentAction;
    if (nCurrentAction == 17 || nCurrentAction == 18 || nCurrentAction == 19 || nCurrentAction == 20 || nCurrentAction == 61 || nCurrentAction == 15 || nCurrentAction == 16 || nCurrentAction == 70 || nCurrentAction == 71)
        pAttack->m_nReactionAnimation = Constants::Animation::Ready;
    else if (pCreature->GetAttackResultHit(pAttack))
        pAttack->m_nReactionAnimation = Constants::Animation::Damage;
    else if (pAttack->m_nAttackResult == 2) // Parry
        pAttack->m_nReactionAnimation = Constants::Animation::Parry;
    else if (pAttack->m_nAttackResult == 4) // Miss
    {
        CNWSCreature* pTargetCreature = nullptr;
        if (pTarget->m_nObjectType == Constants::ObjectType::Creature)
            pTargetCreature = Utils::AsNWSCreature(pTarget);

        if ((pTargetCreature) && (pTargetCreature->GetRangeWeaponEquipped()))
            pAttack->m_nReactionAnimation = Constants::Animation::Dodge;
        else
        {
            switch (rand() % 4)
            {
                case 0: 
                case 1: pAttack->m_nReactionAnimation = Constants::Animation::Ready; break;
                case 2: pAttack->m_nReactionAnimation = Constants::Animation::Dodge; break;
                case 3: pAttack->m_nReactionAnimation = Constants::Animation::Parry; break;
            }
        }
    }

    // Unused
    (void)nAttackIndex;
    (void)nAttacks;
}

static void AddCombatStepActionHook(CNWSCombatRound* pCombatRound, uint32_t nTimeIndex, OBJECT_ID oidTarget)
{
    if (nTimeIndex == 0)
        s_AddCombatStepActionHook->CallOriginal<void>(pCombatRound, nTimeIndex, oidTarget);
}

static void InitializeAttackActionsHook(CNWSCombatRound* pCombatRound, OBJECT_ID oidTarget)
{
    pCombatRound->m_nCurrentAttack = 0; 
    
    auto pCreature = pCombatRound->m_pBaseCreature; 
    if (pCreature->m_nCombatMode == 1) 
    {
        pCreature->BroadcastCombatStateToParty();
        pCombatRound->m_nParryIndex = 0; 
        pCombatRound->m_nParryActions = pCombatRound->GetTotalAttacks();
        return;
    }

    uint32_t nAttackDelay = 6000;
    if (auto nAttackDelayOpt = pCreature->nwnxGet<float>("ATTACK_DELAY"))
        nAttackDelay = (uint32_t)(nAttackDelayOpt.value() * 1000.0f);
    
    int32_t nAttackOffset = 0;
    if (auto nAttackOffsetOpt = pCreature->nwnxGet<int32_t>("ATTACK_OFFSET"))
        nAttackOffset = nAttackOffsetOpt.value();

    int32_t nTotalAnimationTime = 0;
    int32_t nAttackAnimationTime = std::min((int32_t)(nAttackDelay * 0.66f), 750);

    int32_t nTimer = 0;
    if (pCombatRound->GetCombatStepRequired(oidTarget))
    {
        CNWSCombatRoundAction* pStepAction = new CNWSCombatRoundAction();
        pStepAction->m_nActionType = 5;
        pStepAction->m_nActionTimer = nTimer;
        pStepAction->m_oidTarget = oidTarget;
        pStepAction->m_nAnimationTime = 500;
        pStepAction->m_bActionRetargettable = 1;
        pStepAction->m_nAnimation = 0;
        pCombatRound->AddAction(pStepAction);

        nTimer += 500;
    }

    if (nAttackOffset > 0)
        nTimer += nAttackOffset;

    while (nTimer <= 6000)
    {
        CNWSCombatRoundAction* pAttackAction = new CNWSCombatRoundAction();
        pAttackAction->m_nActionType = 1;
        pAttackAction->m_nActionTimer = nTimer;
        pAttackAction->m_oidTarget = oidTarget;
        pAttackAction->m_nNumAttacks = 1;
        pAttackAction->m_nAnimation = 9;
        pAttackAction->m_nAnimationTime = nAttackAnimationTime;
        pAttackAction->m_bActionRetargettable = true;
        pCombatRound->AddAction(pAttackAction);

        nTotalAnimationTime += nAttackAnimationTime;
        nTimer += nAttackDelay;
    }

    pCreature->nwnxSet("ATTACK_OFFSET", std::max(nTimer - 6000, 0));
   
    pCombatRound->DecrementRoundLength(std::min(nTotalAnimationTime, 3000));
}

NWNX_EXPORT ArgumentStack SetCreatureAttackSpeed(ArgumentStack&& args)
{
    if (auto* pCreature = Utils::PopCreature(args))
    {
        auto fAttackSpeed = args.extract<float>();
        auto fAttackDelay = 1.0f / fAttackSpeed;

        pCreature->nwnxSet("ATTACK_DELAY", fAttackDelay, false);
    }

    return {};
}

NWNX_EXPORT ArgumentStack EndCombatRound(ArgumentStack&& args)
{
    if (auto* pCreature = Utils::PopCreature(args))
    {
        bool bOnlyIfMeleeRound = args.extract<int32_t>() != 0;

        auto* pCombatRound = pCreature->m_pcCombatRound;
        if ((pCombatRound->m_bRoundStarted) && ((!bOnlyIfMeleeRound) || (!pCombatRound->m_bSpellCastRound)))
        {
            // TODO: Maybe set ATTACK_OFFSET depending on time until next attack
            pCombatRound->EndCombatRound();
        }
    }

    return {};
}
