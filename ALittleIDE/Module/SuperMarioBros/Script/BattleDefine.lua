-- ALittle Generate Lua And Do Not Edit This Line!
do
if _G.SuperMarioBros == nil then _G.SuperMarioBros = {} end
local ___pairs = pairs
local ___ipairs = ipairs

ALittle.RegStruct(1141499810, "SuperMarioBros.PlayerData", {
name = "SuperMarioBros.PlayerData", ns_name = "SuperMarioBros", rl_name = "PlayerData", hash_code = 1141499810,
name_list = {"level","life","world","subworld","score"},
type_list = {"int","int","int","int","int"},
option_map = {}
})
ALittle.RegStruct(-876795489, "SuperMarioBros.BattleMap", {
name = "SuperMarioBros.BattleMap", ns_name = "SuperMarioBros", rl_name = "BattleMap", hash_code = -876795489,
name_list = {"row_count","col_count","background","object"},
type_list = {"int","int","Map<int,Map<int,int>>","Map<int,Map<int,int>>"},
option_map = {}
})

SuperMarioBros.EntityType = {
	ET_RANDOM_WALL_1 = 1,
	ET_WALL_1 = 2,
	ET_ENEMY_1 = 3,
	ET_HIDE_WALL_1 = 4,
	ET_ENEMY_2 = 5,
	ET_FLAG_1 = 6,
	ET_FLAG_2 = 7,
	ET_FLAG_3 = 8,
	ET_FLAG_4 = 9,
	ET_FLAG_5 = 10,
	ET_BORN_1 = 11,
}

SuperMarioBros.TILE_COL_COUNT = 13
SuperMarioBros.TILE_ROW_COUNT = 11
SuperMarioBros.TILE_WIDTH = 32
SuperMarioBros.TILE_HEIGHT = 32
SuperMarioBros.TILE_TEXTURE_NAME = "tile_map.png"
SuperMarioBros.PLAYER_MAX_WALK_SPEED = 3
SuperMarioBros.PLAYER_INIT_SPEED_RATE = 0.02
SuperMarioBros.PLAYER_MAX_JUMP_HEIGHT = 4 * 32
end