/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
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

#include "buildingtype.h"
#include "version.h"
#include <array>

namespace game {
namespace TBuildingTypeApi {

// clang-format off
static std::array<Api, 4> functions = {{
    // Akella
    Api{
        (Api::Constructor)0x58ba1d,
    },
    // Russobit
    Api{
        (Api::Constructor)0x58ba1d,
    },
    // Gog
    Api{
        (Api::Constructor)0x58ab86,
    },
    // Scenario Editor
    Api{
        (Api::Constructor)0x538599,
    }
}};
// clang-format on

Api& get()
{
    return functions[static_cast<int>(hooks::gameVersion())];
}

} // namespace TBuildingTypeApi

namespace TBuildingUnitUpgTypeApi {

// clang-format off
static std::array<const void*, 4> vftables = {{
    // Akella
    (const void*)0x6ea7cc,
    // Russobit
    (const void*)0x6ea7cc,
    // Gog
    (const void*)0x6e876c,
    // Scenario Editor
    (const void*)0x5df5f4
}};
// clang-format on

const void* vftable()
{
    return vftables[static_cast<int>(hooks::gameVersion())];
}

} // namespace TBuildingUnitUpgTypeApi

} // namespace game
