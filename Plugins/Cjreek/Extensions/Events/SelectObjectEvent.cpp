#include "nwnx.hpp"

#include <unordered_set>
#include "API/CNWSMessage.hpp"
#include "API/CNWSPlayer.hpp"

using namespace NWNXLib;
using namespace NWNXLib::API;

static std::unordered_set<int32_t> s_BlockedInputs
{
    Constants::MessageInputMinor::Attack,
    Constants::MessageInputMinor::ChangeDoorState,
    Constants::MessageInputMinor::UseObject,
    Constants::MessageInputMinor::Dialog,
};

static BOOL HandlePlayerToServerInputMessageHook(CNWSMessage* pMessage, CNWSPlayer* pPlayer, uint8_t nMinor);
static Hooks::Hook s_HandlePlayerToServerInputMessageHook;

void SelectObjectEvent() __attribute__((constructor));
void SelectObjectEvent()
{
    s_HandlePlayerToServerInputMessageHook = Hooks::HookFunction(&CNWSMessage::HandlePlayerToServerInputMessage, &HandlePlayerToServerInputMessageHook, Hooks::Order::Earliest);
}

static BOOL HandlePlayerToServerInputMessageHook(CNWSMessage* pMessage, CNWSPlayer* pPlayer, uint8_t nMinor)
{
    if (s_BlockedInputs.find(nMinor) == s_BlockedInputs.end())
        return s_HandlePlayerToServerInputMessageHook->CallOriginal<BOOL>(pMessage, pPlayer, nMinor);
    else
    {
        auto oidTarget = pMessage->ReadOBJECTIDServer();
        MessageBus::Broadcast("NWNX_EVENT_PUSH_EVENT_DATA", { "TARGET", Utils::ObjectIDToString(oidTarget) });
        MessageBus::Broadcast("NWNX_EVENT_SIGNAL_EVENT", { "NWNX_ON_OBJECT_SELECTED", Utils::ObjectIDToString(pPlayer->m_oidNWSObject) });
        return false;
    }   
}
