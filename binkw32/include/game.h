/*
 * This file is part of the binkw32 proxy dll for Disciples 2.
 * (https://github.com/VladimirMakeev/D2Binkw32Proxy)
 * Copyright (C) 2020 Vladimir Makeev.
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

#ifndef GAME_H
#define GAME_H

namespace game {

struct TRaceType;
struct CPlayerBuildings;
struct LUnitBranch;
struct IdList;
struct CMidDataCache2;
struct CMidgardID;

/** Sets initial values for 'show resources' and 'minimap mode' toggle buttons. */
using RespopupInitFunc = void (*)(void);

/** Sets initial value for 'show banners' toggle button. */
using ToggleShowBannersInitFunc = void*(__thiscall*)(void* thisptr);

/**
 * Assumption: processes unit modifiers during battle.
 * Called with unit pointer being a result of dynamic_cast without a nullptr checks.
 * Main reason of crashes in battles when summoners are involved.
 */
using ProcessUnitModifiersFunc = bool(__thiscall*)(void* unit, int* a2);

/**
 * Adds tier-1 unit from specified unit branch to player's hire list.
 * @param[in] race player race.
 * @param[in] buildings player buildings.
 * @param[in] branch determines unit to add.
 * @param[inout] hireList list to store results.
 * @returns unknown.
 */
using AddUnitToHireList = void*(__stdcall*)(TRaceType* race,
                                            CPlayerBuildings* buildings,
                                            const LUnitBranch* branch,
                                            IdList* hireList);

/**
 * Adds unit from sideshow branch to player's hire list.
 * @param[in] race player race.
 * @param[in] buildings player buildings.
 * @param[inout] hireList list to store results.
 * @returns unknown.
 */
using AddSideshowUnitToHireList = void*(__stdcall*)(TRaceType* race,
                                                    CPlayerBuildings* buildings,
                                                    IdList* hireList);

/**
 * Adds player's tier-1 units to hire list.
 * @param[in] dataCache pointer to data cache object.
 * @param[in] playerId id of player to process.
 * @param[in] a3 meaning unknown.
 * @param[inout] hireList list to store results.
 * @returns true if units were added to the list.
 */
using AddPlayerUnitsToHireList = bool(__stdcall*)(CMidDataCache2* dataCache,
                                                  const CMidgardID* playerId,
                                                  const CMidgardID* a3,
                                                  IdList* hireList);

/** Game functions that can be hooked. */
struct Functions
{
    RespopupInitFunc respopupInit;
    ToggleShowBannersInitFunc toggleShowBannersInit;
    ProcessUnitModifiersFunc processUnitModifiers;
    AddUnitToHireList addUnitToHireList;
    AddSideshowUnitToHireList addSideshowUnitToHireList;
    AddPlayerUnitsToHireList addPlayerUnitsToHireList;
};

/** Global variables used in game. */
struct Variables
{
    int* minimapMode; /**< Value of 0 shows minimap, 1 shows percentage of land converted. */
    unsigned char* respopup;
    int* spellMinLevel;
};

/** Returns functions according to determined version of the game. */
Functions& gameFunctions();

/** Returns variables according to determined version of the game. */
Variables& gameVariables();

} // namespace game

#endif // GAME_H
