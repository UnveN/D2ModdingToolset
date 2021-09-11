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

#ifndef MIDCONDSCRIPT_H
#define MIDCONDSCRIPT_H

namespace game {
struct CMidEvCondition;
struct String;
struct IMidgardObjectMap;
struct LEventCondCategory;
struct ITask;
struct CMidgardID;
struct ITestCondition;

namespace editor {
struct CCondInterf;
}

} // namespace game

namespace hooks {

game::CMidEvCondition* createMidCondScript();

void __stdcall midCondScriptGetInfoString(game::String* info,
                                          const game::IMidgardObjectMap* objectMap,
                                          const game::CMidEvCondition* eventCondition);

game::editor::CCondInterf* createCondScriptInterf(game::ITask* task,
                                                  void* a2,
                                                  const game::CMidgardID* eventId);

game::ITestCondition* createTestScript(game::CMidEvCondition* eventCondition);

} // namespace hooks

#endif // MIDCONDSCRIPT_H
