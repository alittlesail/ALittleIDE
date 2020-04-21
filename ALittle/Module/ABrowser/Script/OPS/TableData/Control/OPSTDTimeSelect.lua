
module("ABrowser", package.seeall)

local ___rawset = rawset
local ___pairs = pairs
local ___ipairs = ipairs
local ___all_struct = ALittle.GetAllStruct()

ALittle.RegStruct(-1479093282, "ALittle.UIEvent", {
name = "ALittle.UIEvent", ns_name = "ALittle", rl_name = "UIEvent", hash_code = -1479093282,
name_list = {"target"},
type_list = {"ALittle.DisplayObject"},
option_map = {}
})
ALittle.RegStruct(-644464135, "ALittle.UIFocusInEvent", {
name = "ALittle.UIFocusInEvent", ns_name = "ALittle", rl_name = "UIFocusInEvent", hash_code = -644464135,
name_list = {"target"},
type_list = {"ALittle.DisplayObject"},
option_map = {}
})

assert(ABrowser.OPSTDBase, " extends class:ABrowser.OPSTDBase is nil")
OPSTDTimeSelect = Lua.Class(ABrowser.OPSTDBase, "ABrowser.OPSTDTimeSelect")

function OPSTDTimeSelect:Ctor(ctrl_sys, center, field_config)
	___rawset(self, "_center", center)
	___rawset(self, "_field_config", field_config)
	if self._field_config.select_day then
		___rawset(self, "_select_type", OPSDateSelectType.SELECT_DAY)
	end
	local width = 0.0
	local height = 0.0
	local title = g_Control:CreateControl("ops_common_text")
	title.text = self._field_config.name
	title.y_type = 3
	local input = g_Control:CreateControl("ops_common_input")
	input.x = title.width
	if self._field_config.width == nil then
		input.width = 190
	else
		input.width = self._field_config.width
	end
	input.y_type = 3
	input:AddEventListener(___all_struct[-644464135], self, self.HandleTimeInput)
	___rawset(self, "_input", input)
	if self._field_config.default_value ~= nil then
		if self._field_config.default_value ~= "" then
			self._input.text = self._field_config.default_value
		else
			if self._select_type == OPSDateSelectType.SELECT_DAY then
				self._input.text = os.date("%Y-%m-%d", os.time())
			else
				self._input.text = os.date("%Y-%m-%d %H:%M:%S", os.time())
			end
		end
	end
	self.width = input.x + input.width
	self.height = input.height
	self:AddChild(title)
	self:AddChild(input)
end

function OPSTDTimeSelect:HandleTimeInput(event)
	self._center.tool_logic:ShowDateSelect(event.target, self._select_type, self._field_config.default_hour, self._field_config.default_min, self._field_config.default_sec)
end

function OPSTDTimeSelect:GetData()
	local text = self._input.text
	local time = OPSDate.StringToTime(text)
	if time == nil then
		return false, nil, self._field_config.name .. " 格式错误"
	end
	local map = {}
	map[self._field_config.query_param] = time
	return true, map, nil
end

function OPSTDTimeSelect:CanOption()
	return self._field_config.option == true and self._input.text == ""
end

function OPSTDTimeSelect:GetCopyString()
	return self._input.text
end

