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

#include "netcustomplayerserver.h"
#include "log.h"
#include "mempool.h"
#include "mqnetreception.h"
#include "mqnetsystem.h"
#include "netcustomplayer.h"
#include "netcustomsession.h"
#include "netmsg.h"
#include "utils.h"
#include <BitStream.h>
#include <MessageIdentifiers.h>
#include <algorithm>
#include <fmt/format.h>
#include <mutex>

namespace hooks {

static std::mutex netMessagesMutex;

void PlayerServerCallbacks::onPacketReceived(DefaultMessageIDTypes type,
                                             SLNet::RakPeerInterface* peer,
                                             const SLNet::Packet* packet)
{
    auto netSystem{playerServer->player.getSystem()};

    switch (type) {
    case ID_REMOTE_DISCONNECTION_NOTIFICATION: {
        logDebug("lobby.log", "PlayerServer: Client disconnected");

        if (netSystem) {
            auto guid = peer->GetGuidFromSystemAddress(packet->systemAddress);
            auto guidInt = SLNet::RakNetGUID::ToUint32(guid);

            netSystem->vftable->onPlayerDisconnected(netSystem, (int)guidInt);
        }

        break;
    }
    case ID_REMOTE_CONNECTION_LOST: {
        logDebug("lobby.log", "PlayerServer: Client lost connection");

        auto guid = peer->GetGuidFromSystemAddress(packet->systemAddress);

        auto& ids = playerServer->connectedIds;
        ids.erase(std::remove(ids.begin(), ids.end(), guid), ids.end());

        if (netSystem) {
            auto guidInt = SLNet::RakNetGUID::ToUint32(guid);
            netSystem->vftable->onPlayerDisconnected(netSystem, (int)guidInt);
        }

        break;
    }
    case ID_REMOTE_NEW_INCOMING_CONNECTION:
        logDebug("lobby.log", "PlayerServer: Client connected");
        break;
    case ID_CONNECTION_REQUEST_ACCEPTED:
        // This should never happen on server ?
        logDebug("lobby.log", "PlayerServer: Connection request to the server was accepted");
        break;
    case ID_NEW_INCOMING_CONNECTION: {
        auto guid = peer->GetGuidFromSystemAddress(packet->systemAddress);
        auto guidInt = SLNet::RakNetGUID::ToUint32(guid);

        logDebug("lobby.log", fmt::format("PlayerServer: Incoming connection, id 0x{:x}", guidInt));

        playerServer->connectedIds.push_back(guid);

        if (netSystem) {
            logDebug("lobby.log", "PlayerServer: Call netSystem onPlayerConnected");
            netSystem->vftable->onPlayerConnected(netSystem, (int)guidInt);
        } else {
            logDebug("lobby.log",
                     "PlayerServer: no netSystem is set, skip onPlayerConnected notification");
        }

        break;
    }
    case ID_NO_FREE_INCOMING_CONNECTIONS:
        // This should never happen on server ?
        logDebug("lobby.log", "PlayerServer: Server is full");
        break;
    case ID_DISCONNECTION_NOTIFICATION: {
        logDebug("lobby.log", "PlayerServer: Client has disconnected from server");

        if (netSystem) {
            auto guid = peer->GetGuidFromSystemAddress(packet->systemAddress);
            auto guidInt = SLNet::RakNetGUID::ToUint32(guid);

            netSystem->vftable->onPlayerDisconnected(netSystem, (int)guidInt);
        }

        break;
    }
    case ID_CONNECTION_LOST: {
        logDebug("lobby.log", "PlayerServer: Client has lost connection");

        if (netSystem) {
            auto guid = peer->GetGuidFromSystemAddress(packet->systemAddress);
            auto guidInt = SLNet::RakNetGUID::ToUint32(guid);

            netSystem->vftable->onPlayerDisconnected(netSystem, (int)guidInt);
        }

        break;
    }
    case 0xff: {
        // Game message received
        auto message = reinterpret_cast<const game::NetMessageHeader*>(packet->data);

        auto guid = peer->GetGuidFromSystemAddress(packet->systemAddress);
        auto guidInt = SLNet::RakNetGUID::ToUint32(guid);

        /*logDebug("playerServer.log", fmt::format("Game message '{:s}' from {:x}",
                                                 message->messageClassName, guidInt));*/

        // logDebug("playerServer.log", "Allocate net message");

        auto msg = std::make_unique<unsigned char[]>(message->length);
        // logDebug("playerServer.log", "Copy net message");
        std::memcpy(msg.get(), message, message->length);

        {
            std::lock_guard<std::mutex> messageGuard(netMessagesMutex);
            playerServer->messages.push_back(
                CNetCustomPlayerServer::IdMessagePair{std::uint32_t{guidInt}, std::move(msg)});
        }

        auto reception = playerServer->player.getReception();
        if (reception) {
            reception->vftable->notify(reception);
        }

        break;
    }
    default:
        logDebug("lobby.log",
                 fmt::format("PlayerServer: Packet type {:d}", static_cast<int>(packet->data[0])));
        break;
    }
}

static void __fastcall playerServerDtor(CNetCustomPlayerServer* thisptr, int /*%edx*/, char flags)
{
    playerLog("CNetCustomPlayerServer d-tor");

    thisptr->~CNetCustomPlayerServer();

    if (flags & 1) {
        playerLog("CNetCustomPlayerServer d-tor frees memory");
        game::Memory::get().freeNonZero(thisptr);
    }
}

static game::String* __fastcall playerServerGetName(CNetCustomPlayerServer* thisptr,
                                                    int /*%edx*/,
                                                    game::String* string)
{
    playerLog("CNetCustomPlayerServer getName");
    thisptr->player.vftable->getName(&thisptr->player, string);
    return string;
}

static int __fastcall playerServerGetNetId(CNetCustomPlayerServer* thisptr, int /*%edx*/)
{
    playerLog("CNetCustomPlayerServer getNetId");
    return thisptr->player.vftable->getNetId(&thisptr->player);
}

static game::IMqNetSession* __fastcall playerServerGetSession(CNetCustomPlayerServer* thisptr,
                                                              int /*%edx*/)
{
    playerLog("CNetCustomPlayerServer getSession");
    return thisptr->player.vftable->getSession(&thisptr->player);
}

static int __fastcall playerServerGetMessageCount(CNetCustomPlayerServer* thisptr, int /*%edx*/)
{
    playerLog("CNetCustomPlayerServer getMessageCount");

    std::lock_guard<std::mutex> messageGuard(netMessagesMutex);

    return static_cast<int>(thisptr->messages.size());
}

static bool __fastcall playerServerSendMessage(CNetCustomPlayerServer* thisptr,
                                               int /*%edx*/,
                                               int idTo,
                                               const game::NetMessageHeader* message)
{
    auto peer{thisptr->getPeer()};

    if (!peer) {
        playerLog("CNetCustomPlayerServer could not send message, peer is nullptr");
        return false;
    }

    const auto& connectedIds{thisptr->connectedIds};
    SLNet::BitStream stream((unsigned char*)message, message->length, false);

    if (idTo == game::broadcastNetPlayerId) {
        playerLog("CNetCustomPlayerServer sendMessage broadcast");

        // Do not use broadcast Send() because player server is also connected to lobby server
        // and we do not want to send him game messages
        for (const auto& guid : connectedIds) {
            peer->Send(&stream, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED,
                       0, guid, false);
        }

        return true;
    }

    auto it = std::find_if(connectedIds.begin(), connectedIds.end(), [idTo](const auto& guid) {
        return static_cast<int>(SLNet::RakNetGUID::ToUint32(guid)) == idTo;
    });

    if (it != connectedIds.end()) {
        const auto& guid = *it;
        playerLog(fmt::format("CNetCustomPlayerServer sendMessage to 0x{:x}",
                              std::uint32_t{SLNet::RakNetGUID::ToUint32(guid)}));

        peer->Send(&stream, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0,
                   guid, false);
        return true;
    }

    playerLog(fmt::format("CNetCustomPlayerServer could not send message. No client with id 0x{:x}",
                          uint32_t(idTo)));
    return false;
}

static int __fastcall playerServerReceiveMessage(CNetCustomPlayerServer* thisptr,
                                                 int /*%edx*/,
                                                 int* idFrom,
                                                 game::NetMessageHeader* buffer)
{
    if (!idFrom || !buffer) {
        // This should never happen
        return 3;
    }

    playerLog("CNetCustomPlayerServer receiveMessage");

    std::lock_guard<std::mutex> messageGuard(netMessagesMutex);

    if (thisptr->messages.empty()) {
        return 0;
    }

    const auto& pair = thisptr->messages.front();
    auto message = reinterpret_cast<const game::NetMessageHeader*>(pair.second.get());

    if (message->messageType != game::netMessageNormalType) {
        playerLog("CNetCustomPlayerServer received message with invalid type");
        return 3;
    }

    if (message->length >= game::netMessageMaxLength) {
        playerLog("CNetCustomPlayerServer received message with invalid length");
        return 3;
    }

    playerLog(fmt::format("CNetCustomPlayerServer receiveMessage '{:s}' length {:d} from 0x{:x}",
                          message->messageClassName, message->length, pair.first));

    *idFrom = static_cast<int>(pair.first);
    std::memcpy(buffer, message, message->length);

    thisptr->messages.pop_front();
    return 2;
}

static void __fastcall playerServerSetNetSystem(CNetCustomPlayerServer* thisptr,
                                                int /*%edx*/,
                                                game::IMqNetSystem* netSystem)
{
    playerLog("CNetCustomPlayerServer setNetSystem");
    thisptr->player.vftable->setNetSystem(&thisptr->player, netSystem);
}

static int __fastcall playerServerMethod8(CNetCustomPlayerServer* thisptr, int /*%edx*/, int a2)
{
    playerLog("CNetCustomPlayerServer method8");
    return thisptr->player.vftable->method8(&thisptr->player, a2);
}

static bool __fastcall playerServerDestroyPlayer(CNetCustomPlayerServer* thisptr,
                                                 int /*%edx*/,
                                                 int playerId)
{
    playerLog("CNetCustomPlayerServer destroyPlayer");
    return false;
}

static bool __fastcall playerServerSetMaxPlayers(CNetCustomPlayerServer* thisptr,
                                                 int /*%edx*/,
                                                 int maxPlayers)
{
    playerLog(fmt::format("CNetCustomPlayerServer setMaxPlayers {:d}", maxPlayers));
    return thisptr->player.getSession()->setMaxPlayers(maxPlayers);
}

static bool __fastcall playerServerSetAllowJoin(CNetCustomPlayerServer* thisptr,
                                                int /*%edx*/,
                                                bool allowJoin)
{
    // Ignore this since its only called during server creation and eventually being allowed
    playerLog(fmt::format("CNetCustomPlayerServer setAllowJoin {:d}", (int)allowJoin));
    return true;
}

static game::IMqNetPlayerServerVftable playerServerVftable{
    (game::IMqNetPlayerServerVftable::Destructor)playerServerDtor,
    (game::IMqNetPlayerServerVftable::GetName)playerServerGetName,
    (game::IMqNetPlayerServerVftable::GetNetId)playerServerGetNetId,
    (game::IMqNetPlayerServerVftable::GetSession)playerServerGetSession,
    (game::IMqNetPlayerServerVftable::GetMessageCount)playerServerGetMessageCount,
    (game::IMqNetPlayerServerVftable::SendNetMessage)playerServerSendMessage,
    (game::IMqNetPlayerServerVftable::ReceiveMessage)playerServerReceiveMessage,
    (game::IMqNetPlayerServerVftable::SetNetSystem)playerServerSetNetSystem,
    (game::IMqNetPlayerServerVftable::Method8)playerServerMethod8,
    (game::IMqNetPlayerServerVftable::DestroyPlayer)playerServerDestroyPlayer,
    (game::IMqNetPlayerServerVftable::SetMaxPlayers)playerServerSetMaxPlayers,
    (game::IMqNetPlayerServerVftable::SetAllowJoin)playerServerSetAllowJoin,
};

CNetCustomPlayerServer::CNetCustomPlayerServer(CNetCustomSession* session,
                                               game::IMqNetSystem* netSystem,
                                               game::IMqNetReception* netReception,
                                               NetworkPeer::PeerPtr&& peer)
    // 1 is a server netId hardcoded in game and was also used in DirectPlay.
    : player{session, netSystem, netReception, "SERVER", std::move(peer), 1}
    , callbacks{this}
{
    vftable = &playerServerVftable;
    player.getPeer().addCallback(&callbacks);
}

bool CNetCustomPlayerServer::notifyHostClientConnected()
{
    auto system = player.getSystem();
    if (!system) {
        playerLog("PlayerServer: no netSystem in notifyHostClientConnected()");
        return false;
    }

    if (connectedIds.empty()) {
        playerLog("PlayerServer: host client is not connected");
        return false;
    }

    const std::uint32_t hostClientNetId{SLNet::RakNetGUID::ToUint32(connectedIds[0])};
    playerLog(fmt::format("PlayerServer: onPlayerConnected 0x{:x}", hostClientNetId));

    system->vftable->onPlayerConnected(system, (int)hostClientNetId);
    return true;
}

game::IMqNetPlayerServer* createCustomPlayerServer(CNetCustomSession* session,
                                                   game::IMqNetSystem* netSystem,
                                                   game::IMqNetReception* netReception)
{
    using namespace game;

    playerLog("Creating player server peer");
    SLNet::SocketDescriptor descriptor{CNetCustomPlayer::serverPort, nullptr};
    auto peer{NetworkPeer::PeerPtr(SLNet::RakPeerInterface::GetInstance())};

    constexpr std::uint16_t maxConnections{4};
    peer->SetMaximumIncomingConnections(maxConnections);

    const auto result{peer->Startup(maxConnections, &descriptor, 1)};
    if (result != SLNet::StartupResult::RAKNET_STARTED) {
        playerLog("Failed to start peer for CNetCustomPlayerServer");
        return nullptr;
    }

    playerLog("Creating CNetCustomPlayerServer");
    auto server = (CNetCustomPlayerServer*)Memory::get().allocate(sizeof(CNetCustomPlayerServer));
    new (server) CNetCustomPlayerServer(session, netSystem, netReception, std::move(peer));

    playerLog("Player server created");

    auto netId = SLNet::RakNetGUID::ToUint32(server->getPeer()->GetMyGUID());
    playerLog(fmt::format("CNetCustomPlayerServer has netId 0x{:x}, ", netId));

    return server;
}

} // namespace hooks
