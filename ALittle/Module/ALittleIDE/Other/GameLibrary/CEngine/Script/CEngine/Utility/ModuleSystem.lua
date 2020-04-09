
module("ALittle", package.seeall)

local ___rawset = rawset
local ___pairs = pairs
local ___ipairs = ipairs

RegStruct(1376035901, "ALittle.ModuleInfo", {
name = "ALittle.ModuleInfo", ns_name = "ALittle", rl_name = "ModuleInfo", hash_code = 1376035901,
name_list = {"name","crypt_mode","control","module","plugin_loaded","module_loaded","browser_loaded","layer_group","browser_setup","browser_addmodule","browser_shutdown","module_setup","module_shutdown","module_getinfo","plugin_setup","plugin_shutdown"},
type_list = {"string","bool","ALittle.ControlSystem","any","bool","bool","bool","ALittle.DisplayLayout","Functor<(ALittle.DisplayLayout,ALittle.ControlSystem,string,string,string)>","Functor<(string,ALittle.DisplayLayout,ALittle.ModuleShortInfo):bool>","Functor<()>","Functor<(ALittle.DisplayLayout,ALittle.ControlSystem,string,string,string)>","Functor<()>","Functor<(ALittle.ControlSystem,string):ALittle.ModuleShortInfo>","Functor<(ALittle.ControlSystem,string,string,string)>","Functor<()>"},
option_map = {}
})
RegStruct(-1652314301, "ALittle.ModuleShortInfo", {
name = "ALittle.ModuleShortInfo", ns_name = "ALittle", rl_name = "ModuleShortInfo", hash_code = -1652314301,
name_list = {"title","icon","width_type","width_value","height_type","height_value"},
type_list = {"string","string","int","int","int","int"},
option_map = {}
})

ModuleSystem = Lua.Class(nil, "ALittle.ModuleSystem")

function ModuleSystem:Ctor()
	___rawset(self, "_name_module_map", {})
	___rawset(self, "_main_module", nil)
	___rawset(self, "_debug", nil)
end

function ModuleSystem.__getter:debug()
	return self._debug
end

function ModuleSystem:GetMainModuleName()
	if self._main_module == nil then
		return nil
	end
	return self._main_module.name
end

function ModuleSystem:LoadModuleImpl(module_base_path, name)
local ___COROUTINE = coroutine.running()
	if package.loaded[name] ~= nil then
		return nil
	end
	local version_system = VersionSystem.CreateVersionSystem("", name)
	version_system:UpdateModule()
	local info = {}
	info.name = name
	info.crypt_mode = (File_ReadTextFromFile("Module/" .. name .. "/NoCrypt.ali", false) == nil)
	info.control = ControlSystem(name, info.crypt_mode)
	Require(module_base_path .. "Main")
	info.module = package.loaded[name]
	if info.module == nil then
		return nil
	end
	self._name_module_map[name] = info
	info.browser_setup = info.module["__Browser_Setup"]
	info.browser_addmodule = info.module["__Browser_AddModule"]
	info.browser_shutdown = info.module["__Browser_Shutdown"]
	info.module_setup = info.module["__Module_Setup"]
	info.module_shutdown = info.module["__Module_Shutdown"]
	info.module_getinfo = info.module["__Module_GetInfo"]
	info.plugin_setup = info.module["__Plugin_Setup"]
	info.plugin_shutdown = info.module["__Plugin_Shutdown"]
	local control = info.control
	if self._main_module ~= nil then
		control = self._main_module.control
	end
	info.layer_group = DisplayLayout(control)
	return info
end

function ModuleSystem:RemoveModule(name)
	if self._main_module ~= nil and self._main_module.name == name then
		return false
	end
	local info = self._name_module_map[name]
	if info == nil then
		return false
	end
	local module_shutdown = info.module_shutdown
	if module_shutdown ~= nil then
		module_shutdown()
	end
	self._name_module_map[name] = nil
	A_LayerManager:RemoveChild(info.layer_group)
	package.loaded[name] = nil
	_G[name] = nil
	collectgarbage("collect")
	return true
end

function ModuleSystem:LoadPlugin(module_base_path, module_name)
local ___COROUTINE = coroutine.running()
	if module_name == nil then
		Log("module_name is null!")
		return false
	end
	local info = self._name_module_map[module_name]
	if info == nil then
		info = self:LoadModuleImpl(module_base_path, module_name)
		if info == nil then
			Log("Module:" .. module_name .. " load failed!")
			return false
		end
	end
	if info.plugin_loaded then
		Log(module_name .. ":__Plugin_Setup already invoked!")
		return false
	end
	local setup_func = info.plugin_setup
	if setup_func == nil then
		Log("can't find Plugin_Setup funciton in Module:" .. module_name)
		return false
	end
	info.plugin_loaded = true
	setup_func(info.control, module_base_path, module_base_path .. "Script/", self._debug)
	return true
end

function ModuleSystem:LoadModule(module_base_path, module_name)
local ___COROUTINE = coroutine.running()
	if module_name == nil then
		Log("module_name is null!")
		return false
	end
	local info = self._name_module_map[module_name]
	if info == nil then
		info = self:LoadModuleImpl(module_base_path, module_name)
		if info == nil then
			Log("Module:" .. module_name .. " load failed!")
			return false
		end
	end
	if info.module_loaded then
		Log(module_name .. ":__Module_Setup already invoked!")
		return false
	end
	local setup_func = info.module_setup
	if setup_func == nil then
		Log("can't find Module_Setup funciton in Module:" .. module_name)
		return false
	end
	info.module_loaded = true
	if self._main_module.name ~= module_name then
		local result = false
		if self._main_module ~= nil and self._main_module.browser_addmodule ~= nil then
			local module_info = nil
			if info.module_getinfo ~= nil then
				module_info = info.module_getinfo(info.control, module_base_path)
			end
			result = self._main_module.browser_addmodule(module_name, info.layer_group, module_info)
		end
		if not result then
			A_LayerManager:AddChild(info.layer_group, A_LayerManager.group_count - 1)
		end
	end
	setup_func(info.layer_group, info.control, module_base_path, module_base_path .. "Script/", self._debug)
	return true
end

function ModuleSystem:MainSetup(base_path, debug, module_name, sengine_path, server_modules)
	if sengine_path ~= nil and sengine_path ~= "" and server_modules ~= nil and server_modules ~= "" then
		local module_map = {}
		local module_list = String_Split(server_modules, ";")
		for index, module in ___ipairs(module_list) do
			local split = String_Split(module, ",")
			if table.maxn(split) == 2 then
				module_map[split[1]] = split[2]
			end
		end
		System_StartServerSystem(sengine_path, module_map)
	end
	if self._main_module ~= nil then
		return
	end
	if module_name == nil then
		module_name = File_ReadTextFromFile("Enter.ali", false)
	end
	if module_name == nil then
		Log("Load Enter.ali failed!")
		return
	end
	local info = self:LoadModuleImpl(base_path, module_name)
	if info == nil then
		Log("Module:" .. module_name .. " load failed!")
		return
	end
	A_LayerManager:AddChild(info.layer_group, A_LayerManager.group_count - 1)
	self._main_module = info
	self._debug = debug
	local module_base_path = "Module/" .. module_name .. "/"
	self._main_module.browser_loaded = true
	local setup_func = self._main_module.browser_setup
	if setup_func == nil then
		self:LoadModule(module_base_path, self._main_module.name)
		return
	end
	setup_func(self._main_module.layer_group, self._main_module.control, module_base_path, module_base_path .. "Script/", self._debug)
end
ModuleSystem.MainSetup = Lua.CoWrap(ModuleSystem.MainSetup)

function ModuleSystem:MainShutdown()
	if self._main_module == nil then
		return
	end
	for name, info in ___pairs(self._name_module_map) do
		local shutdown_func = info.module_shutdown
		if shutdown_func ~= nil and info.module_loaded then
			local error = Lua.TCall(shutdown_func)
			if error ~= nil then
				Log("ModuleSystem:MainShutdown invoke __Module_Shutdown error:", error)
			end
		end
		shutdown_func = info.plugin_shutdown
		if shutdown_func ~= nil and info.plugin_loaded then
			local error = Lua.TCall(shutdown_func)
			if error ~= nil then
				Log("ModuleSystem:MainShutdown invoke __Plugin_Shutdown error:", error)
			end
		end
	end
	local shutdown_func = self._main_module.browser_shutdown
	if shutdown_func == nil then
		return
	end
	shutdown_func()
end

_G.A_ModuleSystem = ModuleSystem()