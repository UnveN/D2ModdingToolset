/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2024 Stanislav Egorov.
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

#ifndef MENUCUSTOMRANDOMSCENARIOMULTI_H
#define MENUCUSTOMRANDOMSCENARIOMULTI_H

#include "dynamiccast.h"
#include "menucustombase.h"
#include "menurandomscenariomulti.h"
#include "netcustomservice.h"
#include <RoomsPlugin.h>

namespace hooks {

class CMenuCustomRandomScenarioMulti
    : public CMenuRandomScenarioMulti
    , public CMenuCustomBase
{
public:
    CMenuCustomRandomScenarioMulti(game::CMenuPhase* menuPhase);
    ~CMenuCustomRandomScenarioMulti();

protected:
    static game::RttiInfo<game::CMenuBaseVftable> rttiInfo;

    static void createRoomAndServer(CMenuRandomScenario* menu);

    // CInterface
    static void __fastcall destructor(CMenuCustomRandomScenarioMulti* thisptr,
                                      int /*%edx*/,
                                      char flags);

    class PeerCallback : public NetPeerCallback
    {
    public:
        PeerCallback(CMenuCustomRandomScenarioMulti* menu)
            : m_menu{menu}
        { }

        ~PeerCallback() override = default;

        void onPacketReceived(DefaultMessageIDTypes type,
                              SLNet::RakPeerInterface* peer,
                              const SLNet::Packet* packet) override;

    private:
        CMenuCustomRandomScenarioMulti* m_menu;
    };

    class RoomsCallback : public SLNet::RoomsCallback
    {
    public:
        RoomsCallback(CMenuCustomRandomScenarioMulti* menu)
            : m_menu{menu}
        { }

        void CreateRoom_Callback(const SLNet::SystemAddress& senderAddress,
                                 SLNet::CreateRoom_Func* callResult) override;

    private:
        CMenuCustomRandomScenarioMulti* m_menu;
    };

private:
    PeerCallback m_peerCallback;
    RoomsCallback m_roomsCallback;
};

assert_offset(CMenuCustomRandomScenarioMulti, vftable, 0);

} // namespace hooks

#endif // MENUCUSTOMRANDOMSCENARIOMULTI_H
