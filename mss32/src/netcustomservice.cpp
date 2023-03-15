/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "netcustomservice.h"
#include "lobbyclient.h"
#include "log.h"
#include "mempool.h"
#include "midgard.h"
#include "mqnetservice.h"
#include "netcustomsession.h"
#include "settings.h"
#include "utils.h"
#include <MessageIdentifiers.h>
#include <fmt/format.h>
#include <mutex>

namespace hooks {

void LobbyPeerCallbacks::onPacketReceived(DefaultMessageIDTypes type,
                                          SLNet::RakPeerInterface* peer,
                                          const SLNet::Packet* packet)
{
    if (!netService->lobbyPeer.peer) {
        return;
    }

    switch (type) {
    case ID_DISCONNECTION_NOTIFICATION:
        logDebug("lobby.log", "Disconnected");
        break;
    case ID_ALREADY_CONNECTED:
        logDebug("lobby.log", "Already connected");
        break;
    case ID_CONNECTION_LOST:
        logDebug("lobby.log", "Connection lost");
        break;
    case ID_CONNECTION_ATTEMPT_FAILED:
        logDebug("lobby.log", "Connection attempt failed");
        break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
        logDebug("lobby.log", "Server is full");
        break;
    case ID_CONNECTION_REQUEST_ACCEPTED: {
        logDebug("lobby.log", "Connection request accepted, set server address");
        // Make sure plugins know about the server
        netService->lobbyClient.SetServerAddress(packet->systemAddress);
        netService->roomsClient.SetServerAddress(packet->systemAddress);
        break;
    }
    case ID_LOBBY2_SERVER_ERROR:
        logDebug("lobby.log", "Lobby server error");
        break;
    default:
        logDebug("lobby.log", fmt::format("Packet type {:d}", static_cast<int>(type)));
        break;
    }
}

CNetCustomService::CNetCustomService(NetworkPeer::PeerPtr&& peer)
    : lobbyPeer(std::move(peer))
    , callbacks(this)
    , session{nullptr}
{
    lobbyPeer.addCallback(&callbacks);

    logDebug("lobby.log", "Set msg factory");
    lobbyClient.SetMessageFactory(&lobbyMsgFactory);

    logDebug("lobby.log", "Create callbacks");
    lobbyClient.SetCallbackInterface(&loggingCallbacks);

    logDebug("lobby.log", "Attach lobby client as a plugin");
    lobbyPeer.peer->AttachPlugin(&lobbyClient);

    lobbyPeer.peer->AttachPlugin(&roomsClient);
    roomsClient.SetRoomsCallback(&roomsLogCallback);
}

static game::IMqNetService* getNetServiceInterface()
{
    auto midgard = game::CMidgardApi::get().instance();
    return midgard->data->netService;
}

CNetCustomService* getNetService()
{
    auto service = getNetServiceInterface();
    if (!isNetServiceCustom(service)) {
        return nullptr;
    }

    return static_cast<CNetCustomService*>(service);
}

void __fastcall netCustomServiceDtor(CNetCustomService* thisptr, int /*%edx*/, char flags)
{
    logDebug("lobby.log", "CNetCustomService d-tor called");
    thisptr->~CNetCustomService();

    if (flags & 1) {
        logDebug("lobby.log", "CNetCustomService d-tor frees memory");
        game::Memory::get().freeNonZero(thisptr);
    }
}

bool __fastcall netCustomServiceHasSessions(CNetCustomService* thisptr, int /*%edx*/)
{
    logDebug("lobby.log", "CNetCustomService hasSessions called");
    return false;
}

void __fastcall netCustomServiceGetSessions(CNetCustomService* thisptr,
                                            int /*%edx*/,
                                            game::List<game::IMqNetSessEnum*>* sessions,
                                            const GUID* appGuid,
                                            const char* ipAddress,
                                            bool allSessions,
                                            bool requirePassword)
{
    // This method used by vanilla interface.
    // Since we have our custom one, we can ignore it and there is no need to implement.
    logDebug("lobby.log", "CNetCustomService getSessions called");
}

void __fastcall netCustomServiceCreateSession(CNetCustomService* thisptr,
                                              int /*%edx*/,
                                              game::IMqNetSession** netSession,
                                              const GUID* /* appGuid */,
                                              const char* sessionName,
                                              const char* password)
{
    logDebug("lobby.log",
             fmt::format("CNetCustomService createSession called. Name '{:s}'", sessionName));

    // We already created a session, just return it
    *netSession = thisptr->session;
}

void __fastcall netCustomServiceJoinSession(CNetCustomService* thisptr,
                                            int /*%edx*/,
                                            game::IMqNetSession** netSession,
                                            game::IMqNetSessEnum* netSessionEnum,
                                            const char* password)
{
    // This method is used by vanilla interface.
    // Since we are using our custom one, we can join session directly and ignore this method.
    logDebug("lobby.log", "CNetCustomService joinSession called");
}

static game::IMqNetServiceVftable netCustomServiceVftable{
    (game::IMqNetServiceVftable::Destructor)netCustomServiceDtor,
    (game::IMqNetServiceVftable::HasSessions)netCustomServiceHasSessions,
    (game::IMqNetServiceVftable::GetSessions)netCustomServiceGetSessions,
    (game::IMqNetServiceVftable::CreateSession)netCustomServiceCreateSession,
    (game::IMqNetServiceVftable::JoinSession)netCustomServiceJoinSession,
};

game::IMqNetService* createCustomNetService()
{
    using namespace game;

    logDebug("lobby.log", "Get peer instance");
    auto lobbyPeer = NetworkPeer::PeerPtr(SLNet::RakPeerInterface::GetInstance());

    const auto& lobbySettings = userSettings().lobby;
    const auto& clientPort = lobbySettings.client.port;
    SLNet::SocketDescriptor socket{clientPort, nullptr};

    logDebug("lobby.log", fmt::format("Start lobby peer on port {:d}", clientPort));

    if (lobbyPeer->Startup(1, &socket, 1) != SLNet::RAKNET_STARTED) {
        logError("lobby.log", "Failed to start lobby client");
        return nullptr;
    }

    const auto& serverIp = lobbySettings.server.ip;
    const auto& serverPort = lobbySettings.server.port;

    logDebug("lobby.log", fmt::format("Connecting to lobby server with ip '{:s}', port {:d}",
                                      serverIp, serverPort));

    if (lobbyPeer->Connect(serverIp.c_str(), serverPort, nullptr, 0)
        != SLNet::CONNECTION_ATTEMPT_STARTED) {
        logError("lobby.log", "Failed to connect to lobby server");
        return nullptr;
    }

    logDebug("lobby.log", "Allocate CNetCustomService");
    auto netService = (CNetCustomService*)game::Memory::get().allocate(sizeof(CNetCustomService));

    logDebug("lobby.log", "Call placement new");
    new (netService) CNetCustomService(std::move(lobbyPeer));

    logDebug("lobby.log", "Assign vftable");
    netService->vftable = &netCustomServiceVftable;

    logDebug("lobby.log", "CNetCustomService created");
    return netService;
}

void addLobbyCallbacks(SLNet::Lobby2Callbacks* callbacks)
{
    auto netService{getNetService()};
    if (!netService) {
        return;
    }

    netService->lobbyClient.AddCallbackInterface(callbacks);
}

void removeLobbyCallbacks(SLNet::Lobby2Callbacks* callbacks)
{
    auto netService{getNetService()};
    if (!netService) {
        return;
    }

    netService->lobbyClient.RemoveCallbackInterface(callbacks);
}

void addRoomsCallback(SLNet::RoomsCallback* callback)
{
    auto netService{getNetService()};
    if (!netService) {
        return;
    }

    logDebug("lobby.log", fmt::format("Adding room callback {:p}", (void*)callback));
    netService->roomsClient.AddRoomsCallback(callback);
}

void removeRoomsCallback(SLNet::RoomsCallback* callback)
{
    auto netService{getNetService()};
    if (!netService) {
        return;
    }

    logDebug("lobby.log", fmt::format("Removing room callback {:p}", (void*)callback));
    netService->roomsClient.RemoveRoomsCallback(callback);
}

bool isNetServiceCustom()
{
    return isNetServiceCustom(getNetServiceInterface());
}

bool isNetServiceCustom(const game::IMqNetService* service)
{
    return service && (service->vftable == &netCustomServiceVftable);
}

} // namespace hooks
