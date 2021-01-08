-- ALittle Generate Lua And Do Not Edit This Line!
do
if _G.ALittleDeploy == nil then _G.ALittleDeploy = {} end
local ALittleDeploy = ALittleDeploy
local Lua = Lua
local ALittle = ALittle
local ___pairs = pairs
local ___ipairs = ipairs


ALittleDeploy.DPLCenter = Lua.Class(nil, "ALittleDeploy.DPLCenter")

function ALittleDeploy.DPLCenter:Setup()
	local ___COROUTINE = coroutine.running()
	ALittleDeploy.g_Control:CreateControl("dpl_main_scene", self, ALittleDeploy.g_MainLayer)
	A_UISystem.keydown_callback = Lua.Bind(self.HandleShortcutKey, self)
	A_UISystem.quit_callback = Lua.Bind(self.HandleQuit, self)
end

function ALittleDeploy.DPLCenter:Shutdown()
	if self._account ~= nil then
		self._account:Shutdown()
	end
	if self._main_menu ~= nil then
		self._main_menu:Shutdown()
	end
end

function ALittleDeploy.DPLCenter.__getter:center()
	return self._center
end

function ALittleDeploy.DPLCenter:RefreshProject()
	if not self._center.content_edit:IsSaveAll() then
		local result = g_AUITool:SaveNotice("提示", "是否保存当前项目?")
		if result == "YES" then
			self._center.content_edit:SaveAllTab()
		end
	end
end
ALittleDeploy.DPLCenter.RefreshProject = Lua.CoWrap(ALittleDeploy.DPLCenter.RefreshProject)

function ALittleDeploy.DPLCenter:HandleShortcutKey(mod, sym, scancode)
	if A_UISystem.sym_map[1073741886] then
		return
	end
	if A_UISystem.sym_map[1073741887] then
		return
	end
	if A_UISystem.sym_map[1073741891] then
		return
	end
	if self._center ~= nil then
		self._center:HandleShortcutKey()
	end
end

function ALittleDeploy.DPLCenter:HandleQuit()
	if self._center.content_edit:IsSaveAll() then
		return true
	end
	self:HandleQuitImpl()
	return false
end

function ALittleDeploy.DPLCenter:HandleQuitImpl()
	local result = g_AUITool:SaveNotice("提示", "是否保存当前项目?")
	if result == "YES" then
		self._center.content_edit:SaveAllTab()
	end
	ALittle.System_Exit()
end
ALittleDeploy.DPLCenter.HandleQuitImpl = Lua.CoWrap(ALittleDeploy.DPLCenter.HandleQuitImpl)

ALittleDeploy.g_DPLCenter = ALittleDeploy.DPLCenter()
end