/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2022 Vladimir Makeev.
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

#include "menunewskirmishmulti.h"
#include "version.h"
#include <array>

namespace game::CMenuNewSkirmishMultiApi {

// clang-format off
static std::array<Api, 3> functions = {{
    // Akella
    Api{
        (Api::Constructor)0x4ea0d6,
        (Api::Destructor)0x4ea5cf,
        (Api::CreateServer)0x4ea31b,
    },
    // Russobit
    Api{
        (Api::Constructor)0x4ea0d6,
        (Api::Destructor)0x4ea5cf,
        (Api::CreateServer)0x4ea31b,
    },
    // Gog
    Api{
        (Api::Constructor)0x4e9571,
        (Api::Destructor)0x4e9cd8,
        (Api::CreateServer)0x4e97b6,
    },
}};
// clang-format on

Api& get()
{
    return functions[static_cast<int>(hooks::gameVersion())];
}

} // namespace game::CMenuNewSkirmishMultiApi
