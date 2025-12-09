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

#include "landmarkutils.h"
#include "globaldata.h"
#include "landmark.h"
#include "utils.h"
#include <spdlog/spdlog.h>

namespace hooks {

bool isLandmarkMountain(const game::CMidgardID* landmarkTypeId)
{
    using namespace game;

    if (!landmarkTypeId) {
        return false;
    }

    const GlobalData* global = *GlobalDataApi::get().getGlobalData();
    if (!global) {
        spdlog::error("Global data is not available");
        return false;
    }

    if (!global->landmarks || !*global->landmarks) {
        spdlog::error("Landmarks global data is not loaded");
        return false;
    }

    // Получаем доступ к данным landmarks аналогично spells
    const auto& landmarks = (*global->landmarks)->data;

    // Перебираем все landmarks в поисках нужного ID
    for (const auto* i = landmarks.bgn; i != landmarks.end; ++i) {
        const TLandmark* landmark = i->second;

        if (landmark->id == *landmarkTypeId) {
            if (landmark->data) {
                return landmark->data->mountain;
            } else {
                spdlog::error("Global landmark {:s} has null data", idToString(landmarkTypeId));
                return false;
            }
        }
    }

    spdlog::error("Could not find global landmark {:s} in landmarks collection",
                  idToString(landmarkTypeId));
    return false;
}

} // namespace hooks