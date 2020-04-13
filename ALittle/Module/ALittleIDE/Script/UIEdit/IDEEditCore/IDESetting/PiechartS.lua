
module("ALittleIDE", package.seeall)

local ___rawset = rawset
local ___pairs = pairs
local ___ipairs = ipairs
local ___coroutine = coroutine


assert(DisplayLayoutS, " extends class:DisplayLayoutS is nil")
PiechartS = ALittle.Class(DisplayLayoutS, "ALittleIDE.PiechartS")

function PiechartS:Ctor(user_info, tab_child, tree_logic)
	___rawset(self, "_layer_name", "ide_setting_piechart")
end

function PiechartS:LoadNatureBase()
	DisplayLayoutS.LoadNatureBase(self)
	self:LoadDefaultNilString("texture_name")
	self:LoadValueData("start_degree")
	self:LoadValueData("end_degree")
end

function PiechartS:HandleImageTextureNameFOCUSOUT(event)
	self:DefaultNilStringInputChange("texture_name", false)
end

function PiechartS:HandleImageTextureNameSelect(event)
	local func = ALittle.Bind(self.ImagePathSelectCallback, self, "texture_name", self.HandleImageTextureNameFOCUSOUT, nil)
	g_IDEAttrImageDialog:ShowDialog(nil, func)
end

function PiechartS:HandleImageStartDegreeFOCUSOUT(event)
	self:ValueNumZInputChange("start_degree", false)
end

function PiechartS:HandleImageEndDegreeFOCUSOUT(event)
	self:ValueNumZInputChange("end_degree", false)
end
