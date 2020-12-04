-- ALittle Generate Lua And Do Not Edit This Line!
do
if _G.NESEmulator == nil then _G.NESEmulator = {} end
local ___pairs = pairs
local ___ipairs = ipairs


assert(NESEmulator.NesCore, " extends class:NESEmulator.NesCore is nil")
NESEmulator.GNes = Lua.Class(NESEmulator.NesCore, "NESEmulator.GNes")

function NESEmulator.GNes:Setup(image)
	self._image = image
	A_UISystem.keydown_callback = Lua.Bind(self.HandleKey, self, true)
	A_UISystem.keyup_callback = Lua.Bind(self.HandleKey, self, false)
end

function NESEmulator.GNes:LoadROM(file_name)
	self:Reset()
	if self._loop_frame ~= nil then
		self._loop_frame:Stop()
		self._loop_frame = nil
	end
	if self._loop_func ~= nil then
		self._loop_func:Stop()
		self._loop_func = nil
	end
	local path = NESEmulator.g_ModuleBasePath .. "Other/" .. file_name
	local nes_rom = ALittle.LocalFile()
	if not nes_rom:Load(path) then
		return
	end
	local error = self:ReloadROM(nes_rom)
	if error ~= nil then
		ALittle.Log("ReloadROM", error)
		return
	end
	self._loop_frame = ALittle.LoopFrame(Lua.Bind(self.UpdateFrame, self))
	self._loop_frame:Start()
	self._loop_func = ALittle.LoopFunction(Lua.Bind(self.ShowInfo, self), -1, 1000, 1000)
	self._loop_func:Start()
end
NESEmulator.GNes.LoadROM = Lua.CoWrap(NESEmulator.GNes.LoadROM)

function NESEmulator.GNes:HandleKey(down, mod, sym, scancode)
	if sym == 97 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_LEFT)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_LEFT)
		end
	elseif sym == 115 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_DOWN)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_DOWN)
		end
	elseif sym == 100 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_RIGHT)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_RIGHT)
		end
	elseif sym == 119 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_UP)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_UP)
		end
	elseif sym == 103 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_SELECT)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_SELECT)
		end
	elseif sym == 104 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_START)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_START)
		end
	elseif sym == 106 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_A)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_A)
		end
	elseif sym == 107 then
		if down then
			self:ButtonDown(1, NESEmulator.NesControlType.BUTTON_B)
		else
			self:ButtonUp(1, NESEmulator.NesControlType.BUTTON_B)
		end
	end
end

function NESEmulator.GNes:OnFrame(frame_buffer)
	local x = 0
	while true do
		if not(x < NESEmulator.SCREEN_WIDTH) then break end
		local y = 0
		while true do
			if not(y < NESEmulator.SCREEN_HEIGHT) then break end
			local index = y * NESEmulator.SCREEN_WIDTH + x
			frame_buffer[index] = frame_buffer[index] | 0xFF000000
			y = y+(1)
		end
		x = x+(1)
	end
	self._image:SetRangeColor(frame_buffer, NESEmulator.SCREEN_WIDTH, NESEmulator.SCREEN_HEIGHT)
end

function NESEmulator.GNes:ShowInfo()
end

function NESEmulator.GNes:UpdateFrame(frame_time)
	local error = self:Frame()
	if error ~= nil then
		ALittle.Log(error)
		if self._loop_frame ~= nil then
			self._loop_frame:Stop()
		end
		if self._loop_func ~= nil then
			self._loop_func:Stop()
		end
	end
end

function NESEmulator.GNes:Shutdown()
	if self._loop_frame ~= nil then
		self._loop_frame:Stop()
	end
	self._loop_frame = nil
	if self._loop_func ~= nil then
		self._loop_func:Stop()
	end
	self._loop_func = nil
end

_G.g_GNes = NESEmulator.GNes()
end