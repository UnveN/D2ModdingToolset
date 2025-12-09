/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2025 Alexey Voskresensky.
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

#include "landmarkview.h"
#include "landmarkutils.h"
#include "midlandmark.h"
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace bindings {

LandmarkView::LandmarkView(const game::CMidLandmark* landmark,
                           const game::IMidgardObjectMap* objectMap)
    : landmark{landmark}
    , objectMap{objectMap}
{ }

void LandmarkView::bind(sol::state& lua)
{
    auto landmarkView = lua.new_usertype<LandmarkView>("LandmarkView");
    landmarkView["id"] = sol::property(&LandmarkView::getId);
    landmarkView["position"] = sol::property(&LandmarkView::getPosition);
    landmarkView["size"] = sol::property(&LandmarkView::getSize);
    landmarkView["description"] = sol::property(&LandmarkView::getDescription);
    landmarkView["typeId"] = sol::property(&LandmarkView::getTypeId);
    landmarkView["mountain"] = sol::property(&LandmarkView::isMountain);
}

IdView LandmarkView::getId() const
{
    return IdView{landmark->id};
}

Point LandmarkView::getPosition() const
{
    return Point{landmark->mapElement.position};
}

Point LandmarkView::getSize() const
{
    return Point{landmark->mapElement.sizeX, landmark->mapElement.sizeY};
}

std::string LandmarkView::getDescription() const
{
    return landmark->description.string;
}

IdView LandmarkView::getTypeId() const
{
    return IdView{landmark->landmarkTypeId};
}

bool LandmarkView::isMountain() const
{
    if (!landmark) {
        return false;
    }

    return hooks::isLandmarkMountain(&landmark->landmarkTypeId);
}

} // namespace bindings