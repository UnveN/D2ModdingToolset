--[[
For complete API reference see
https://github.com/VladimirMakeev/D2ModdingToolset/blob/master/luaApi.md

If the script is used as SEL_SCRIPT, no unit is selected yet (selected.position == -1).

Unit positions on a battlefield are mirrored.
Frontline positions are even, backline - odd.
1 0    0 1
3 2 vs 2 3
5 4    4 5
--]]

function getTargets(attacker, selected, allies, targets, targetsAreAllies)
	-- Get the selected target and the one behind it (pierce attack)
	local result = {selected}
	for i = 1, #targets do
		local target = targets[i]
		if target.backline and target.position == selected.position + 1 then
			table.insert(result, target)
			break
		end
	end
	return result
end
