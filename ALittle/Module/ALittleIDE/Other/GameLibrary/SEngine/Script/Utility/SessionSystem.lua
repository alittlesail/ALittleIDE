
module("ALittle", package.seeall)

local ___rawset = rawset
local ___pairs = pairs
local ___ipairs = ipairs
local ___all_struct = GetAllStruct()

RegStruct(1715346212, "ALittle.Event", {
name = "ALittle.Event", ns_name = "ALittle", rl_name = "Event", hash_code = 1715346212,
name_list = {"target"},
type_list = {"ALittle.EventDispatcher"},
option_map = {}
})
RegStruct(888437463, "ALittle.SessionConnectEvent", {
name = "ALittle.SessionConnectEvent", ns_name = "ALittle", rl_name = "SessionConnectEvent", hash_code = 888437463,
name_list = {"target","connect_key","route_type","route_num","session"},
type_list = {"ALittle.EventDispatcher","int","int","int","ALittle.MsgSessionTemplate<ALittle.MsgSessionNative,lua.__CPPAPIMessageWriteFactory>"},
option_map = {}
})
RegStruct(-36908822, "ALittle.SessionDisconnectEvent", {
name = "ALittle.SessionDisconnectEvent", ns_name = "ALittle", rl_name = "SessionDisconnectEvent", hash_code = -36908822,
name_list = {"target","connect_key","route_type","route_num","session"},
type_list = {"ALittle.EventDispatcher","int","int","int","ALittle.MsgSessionTemplate<ALittle.MsgSessionNative,lua.__CPPAPIMessageWriteFactory>"},
option_map = {}
})
RegStruct(-1417161474, "ALittle.ConnectSessionInfo", {
name = "ALittle.ConnectSessionInfo", ns_name = "ALittle", rl_name = "ConnectSessionInfo", hash_code = -1417161474,
name_list = {"route_type","route_num","connect_key","is_connecting"},
type_list = {"int","int","int","bool"},
option_map = {}
})

assert(ALittle.IMsgCommonNative, " extends class:ALittle.IMsgCommonNative is nil")
MsgSessionNative = Lua.Class(ALittle.IMsgCommonNative, "ALittle.MsgSessionNative")

function MsgSessionNative:SetID(id)
	self._connect_key = id
end

function MsgSessionNative:GetID()
	return self._connect_key
end

function MsgSessionNative:SendFactory(factory)
	__CPPAPI_ServerSchedule:SessionSend(self._connect_key, factory)
end

function MsgSessionNative:Close()
	__CPPAPI_ServerSchedule:SessionClose(self._connect_key)
	A_SessionSystem:RemoveSessionServer(self._connect_key)
end

MsgSession = Lua.Template(MsgSessionTemplate, "ALittle.MsgSessionTemplate<ALittle.MsgSessionNative, lua.__CPPAPIMessageWriteFactory>", MsgSessionNative, __CPPAPIMessageWriteFactory);
assert(ALittle.EventDispatcher, " extends class:ALittle.EventDispatcher is nil")
SessionSystem = Lua.Class(ALittle.EventDispatcher, "ALittle.SessionSystem")

function SessionSystem:Ctor()
	___rawset(self, "_session_map", {})
	___rawset(self, "_connect_map", {})
	___rawset(self, "_invoke_map", {})
end

function SessionSystem:GetSessionMap()
	return self._session_map
end

function SessionSystem:SendMsgToAll(T, msg)
	for connect_key, session in ___pairs(self._session_map) do
		session:SendMsg(T, msg)
	end
end

function SessionSystem:RemoveSessionServer(connect_key)
	self._session_map[connect_key] = nil
end

function SessionSystem:AddConnectSession(route_type, route_num)
	local key = route_type .. "_" .. route_num
	local info = self._connect_map[key]
	if info ~= nil then
		return
	end
	info = {}
	info.route_type = route_type
	info.route_num = route_num
	info.is_connecting = true
	self._connect_map[key] = info
	__CPPAPI_ServerSchedule:ConnectSession(route_type, route_num)
end

function SessionSystem:ConnectSession(route_type, route_num)
local ___COROUTINE = coroutine.running()
	if ___COROUTINE == nil then
		return "当前不是协程", nil
	end
	local key = route_type .. "_" .. route_num
	local info = self._connect_map[key]
	if info ~= nil and not info.is_connecting then
		local connect_key = __CPPAPI_ServerSchedule:CalcConnectKey(__CPPAPI_ServerSchedule:GetRouteType(), __CPPAPI_ServerSchedule:GetRouteNum(), route_type, route_num)
		local connect = self._session_map[connect_key]
		if connect ~= nil then
			return nil, connect
		end
		connect_key = __CPPAPI_ServerSchedule:CalcConnectKey(route_type, route_num, __CPPAPI_ServerSchedule:GetRouteType(), __CPPAPI_ServerSchedule:GetRouteNum())
		connect = self._session_map[connect_key]
		if connect ~= nil then
			return nil, connect
		end
		return "连接居然不存在", nil
	end
	if info == nil then
		self:AddConnectSession(route_type, route_num)
	end
	local list = self._invoke_map[key]
	if list == nil then
		list = {}
		self._invoke_map[key] = list
	end
	List_Push(list, ___COROUTINE)
	return coroutine.yield()
end

function SessionSystem:HandleSessionConnect(connect_key, route_type, route_num)
	local session = MsgSession(connect_key, route_type, route_num)
	self._session_map[connect_key] = session
	session:HandleConnected()
	local event = {}
	event.target = self
	event.connect_key = connect_key
	event.route_type = route_type
	event.route_num = route_num
	event.session = session
	self:DispatchEvent(___all_struct[888437463], event)
end

function SessionSystem:HandleSessionDisconnect(connect_key, route_type, route_num)
	local session = self._session_map[connect_key]
	if session ~= nil then
		self._session_map[connect_key] = nil
		session:HandleDisconnected()
	end
	local event = {}
	event.target = self
	event.connect_key = connect_key
	event.route_type = route_type
	event.route_num = route_num
	event.session = session
	self:DispatchEvent(___all_struct[888437463], event)
	local key = route_type .. "_" .. route_num
	local info = self._connect_map[key]
	if info ~= nil and info.connect_key == connect_key then
		info.is_connecting = true
		A_LoopSystem:AddTimer(1000, Lua.Bind(self.ConnectSessionImpl, self, info), nil, nil)
	end
end

function SessionSystem:HandleConnectSessionFailed(route_type, route_num, reason)
	local key = route_type .. "_" .. route_num
	local list = self._invoke_map[key]
	if list ~= nil then
		self._invoke_map[key] = nil
		for index, thread in ___ipairs(list) do
			local result, error = coroutine.resume(thread, reason, nil)
			if result ~= true then
				Error(error)
			end
		end
	end
	local info = self._connect_map[key]
	if info ~= nil then
		info.is_connecting = true
		A_LoopSystem:AddTimer(1000, Lua.Bind(self.ConnectSessionImpl, self, info), nil, nil)
	end
end

function SessionSystem:HandleConnectSessionSucceed(connect_key, route_type, route_num)
	local key = route_type .. "_" .. route_num
	local info = self._connect_map[key]
	if info ~= nil then
		info.connect_key = connect_key
		info.is_connecting = false
	end
	local list = self._invoke_map[key]
	if list ~= nil then
		local session = self._session_map[connect_key]
		local error = nil
		if session == nil then
			error = "连接居然不存在"
		end
		self._invoke_map[key] = nil
		for index, thread in ___ipairs(list) do
			local result, reason = coroutine.resume(thread, error, session)
			if result ~= true then
				Error(reason)
			end
		end
	end
end

function SessionSystem:HandleSessionMessage(connect_key, id, rpc_id, factory)
	local session = self._session_map[connect_key]
	if session == nil then
		return
	end
	session:HandleMessage(id, rpc_id, factory)
end

function SessionSystem:ConnectSessionImpl(info)
	__CPPAPI_ServerSchedule:ConnectSession(info.route_type, info.route_num)
end

_G.A_SessionSystem = SessionSystem()
