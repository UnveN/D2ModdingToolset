/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2023 Vladimir Makeev.
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

#ifndef MENURANDOMSCENARIOMULTI_H
#define MENURANDOMSCENARIOMULTI_H

#include "menurandomscenario.h"

namespace game {
struct CMenuBase;
struct CMenuPhase;
} // namespace game

namespace hooks {

struct CMenuRandomScenarioMulti : public CMenuRandomScenario
{
    CMenuRandomScenarioMulti(game::CMenuPhase* menuPhase);
    ~CMenuRandomScenarioMulti() = default;
};

game::CMenuBase* __stdcall createMenuRandomScenarioMulti(game::CMenuPhase* menuPhase);

} // namespace hooks

#endif // MENURANDOMSCENARIOMULTI_H
