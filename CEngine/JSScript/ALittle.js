{


__ALITTLEAPI_CEngine_Init = async function(base_path) {
	{
		await Require(base_path + "Adapter/JavaScript/JHttpInterface");
		await Require(base_path + "Adapter/JavaScript/JHttpFileInterface");
		await Require(base_path + "Adapter/JavaScript/JMessageFactory");
		await Require(base_path + "Adapter/JavaScript/JMsgInterface");
		await Require(base_path + "Adapter/JavaScript/JSystem");
	}
	await Require(base_path + "CEngine/Utility/String");
	await Require(base_path + "CEngine/Utility/File");
	await Require(base_path + "CEngine/Utility/ModuleSystem");
	await Require(base_path + "CEngine/Utility/System");
	await Require(base_path + "CEngine/Utility/ConfigSystem");
	await Require(base_path + "CEngine/Utility/AudioSystem");
	await Require(base_path + "CEngine/Utility/OtherSystem");
	await Require(base_path + "CEngine/Utility/CsvConfigManager");
	await Require(base_path + "CEngine/LoopSystem/LoopAnimation");
	await Require(base_path + "CEngine/LoopSystem/LoopAttribute");
	await Require(base_path + "CEngine/LoopSystem/LoopLinear");
	await Require(base_path + "CEngine/LoopSystem/LoopRit");
	await Require(base_path + "CEngine/UISystem/UIEnumTypes");
	await Require(base_path + "CEngine/UISystem/UIEventDispatcher");
	await Require(base_path + "CEngine/UISystem/Base/Matrix2D");
	await Require(base_path + "CEngine/UISystem/Base/DisplayObject");
	await Require(base_path + "CEngine/UISystem/Base/DisplayGroup");
	await Require(base_path + "CEngine/UISystem/Base/DisplayLayout");
	await Require(base_path + "CEngine/UISystem/UISystem");
	await Require(base_path + "CEngine/UISystem/LayerManager");
	ALittle.System_CreateView("test", 800, 600, 0, 0);
}

__ALITTLEAPI_HandleConsoleCmd = function(cmd) {
	ALittle.ExecuteCommand(cmd);
}

__ALITTLEAPI_FingerMoved = function(x, y, finger_id, touch_id) {
	A_UISystem.HandleFingerMoved(x, y, finger_id, touch_id);
}

__ALITTLEAPI_FingerDown = function(x, y, finger_id, touch_id) {
	A_UISystem.HandleFingerDown(x, y, finger_id, touch_id);
}

__ALITTLEAPI_FingerUp = function(x, y, finger_id, touch_id) {
	A_UISystem.HandleFingerUp(x, y, finger_id, touch_id);
}

__ALITTLEAPI_MouseMoved = function(x, y) {
	A_UISystem.HandleMouseMoved(x, y);
}

__ALITTLEAPI_LButtonDown = function(x, y, count) {
	A_UISystem.HandleLButtonDown(x, y, count);
}

__ALITTLEAPI_LButtonUp = function(x, y) {
	A_UISystem.HandleLButtonUp(x, y);
}

__ALITTLEAPI_MButtonDown = function(x, y, count) {
	A_UISystem.HandleMButtonDown(x, y, count);
}

__ALITTLEAPI_MButtonUp = function(x, y) {
	A_UISystem.HandleMButtonUp(x, y);
}

__ALITTLEAPI_RButtonDown = function(x, y, count) {
	A_UISystem.HandleRButtonDown(x, y, count);
}

__ALITTLEAPI_RButtonUp = function(x, y) {
	A_UISystem.HandleRButtonUp(x, y);
}

__ALITTLEAPI_MouseWheel = function(x, y) {
	A_UISystem.HandleMouseWheel(x, y);
}

__ALITTLEAPI_KeyDown = function(mod, sym, scancode) {
	A_UISystem.HandleKeyDown(mod, sym, scancode);
}

__ALITTLEAPI_KeyUp = function(mod, sym, scancode) {
	A_UISystem.HandleKeyUp(mod, sym, scancode);
}

__ALITTLEAPI_ViewResized = function(width, height) {
	A_UISystem.HandleViewResized(width, height);
}

__ALITTLEAPI_TextInput = function(text) {
	A_UISystem.HandleTextInput(text);
}

__ALITTLEAPI_TextEditing = function(text, start) {
	A_UISystem.HandleTextEditing(text, start);
}

__ALITTLEAPI_DropFile = function(path) {
	A_UISystem.HandleDropFile(path);
}

__ALITTLEAPI_WindowEnter = function() {
	A_UISystem.HandleWindowEnter();
}

__ALITTLEAPI_WindowLeave = function() {
	A_UISystem.HandleWindowLeave();
}

__ALITTLEAPI_Update = function(frame_time) {
	A_JSLoopSystem.Update(frame_time);
}

__ALITTLEAPI_WillEnterBackground = function() {
	A_OtherSystem.HandleWillEnterBackground();
}

__ALITTLEAPI_DidEnterBackground = function() {
	A_OtherSystem.HandleDidEnterBackground();
}

__ALITTLEAPI_WillEnterForeground = function() {
	A_OtherSystem.HandleWillEnterForeground();
}

__ALITTLEAPI_DidEnterForeground = function() {
	A_OtherSystem.HandleDidEnterForeground();
}

__ALITTLEAPI_LowMemory = function() {
	A_OtherSystem.HandleLowMemory();
}

__ALITTLEAPI_ConnectSucceed = function(msg_interface) {
	let client = ALittle.FindMsgSender(msg_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleConnectSucceed();
}

__ALITTLEAPI_Disconnect = function(msg_interface) {
	let client = ALittle.FindMsgSender(msg_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleDisconnect();
}

__ALITTLEAPI_ConnectFailed = function(msg_interface) {
	let client = ALittle.FindMsgSender(msg_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleConnectFailed(undefined);
}

__ALITTLEAPI_Message = function(id, rpc_id, factory, msg_interface) {
	let client = ALittle.FindMsgSender(msg_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleMessage(id, rpc_id, factory);
}

__ALITTLEAPI_HttpClientSucceed = function(http_interface) {
	let client = ALittle.FindHttpSender(http_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleSucceed();
}

__ALITTLEAPI_HttpClientFailed = function(http_interface, reason) {
	let client = ALittle.FindHttpSender(http_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleFailed(reason);
}

__ALITTLEAPI_HttpFileSucceed = function(httpfile_interface) {
	let client = ALittle.FindHttpFileSender(httpfile_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleSucceed();
}

__ALITTLEAPI_HttpFileFailed = function(httpfile_interface, reason) {
	let client = ALittle.FindHttpFileSender(httpfile_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleFailed(reason);
}

__ALITTLEAPI_HttpFileProcess = function(httpfile_interface) {
	let client = ALittle.FindHttpFileSender(httpfile_interface.GetID());
	if (client === undefined) {
		return;
	}
	client.HandleProcess();
}

__ALITTLEAPI_TextureLoadSucceed = function(loader, texture) {
	A_LoadTextureManager.HandleTextureLoadSucceed(loader, texture);
}

__ALITTLEAPI_TextureLoadFailed = function(loader) {
	A_LoadTextureManager.HandleTextureLoadFailed(loader);
}

__ALITTLEAPI_TextureCutLoadSucceed = function(loader, texture) {
	A_LoadTextureManager.HandleTextureCutLoadSucceed(loader, texture);
}

__ALITTLEAPI_TextureCutLoadFailed = function(loader) {
	A_LoadTextureManager.HandleTextureCutLoadFailed(loader);
}

__ALITTLEAPI_CsvFileLoadSucceed = function(loader, file) {
	A_CsvConfigManager.HandleCsvFileLoadSucceed(loader, file);
}

__ALITTLEAPI_CsvFileLoadFailed = function(loader) {
	A_CsvConfigManager.HandleCsvFileLoadFailed(loader);
}

__ALITTLEAPI_RenderDeviceReset = function() {
	A_LoadTextureManager.HandleRenderDeviceReset();
}

__ALITTLEAPI_AudioChunkStopedEvent = function(id) {
	A_AudioSystem.HandleAudioChunkStopedEvent(id);
}

__ALITTLEAPI_NetworkChanged = function(net_type) {
	ALittle.System_ClearAIFamily();
	A_OtherSystem.HandleNetworkChanged(net_type);
}

__ALITTLEAPI_ALittleJsonRPC = function(json) {
	A_OtherSystem.HandleALittleJsonRPC(json);
}

__ALITTLEAPI_SystemSelectFile = function(path) {
	A_OtherSystem.HandleSystemSelectFile(path);
}

__ALITTLEAPI_SystemSaveFile = function(path) {
	A_OtherSystem.HandleSystemSaveFile(path);
}

__ALITTLEAPI_SetupMainModule = async function(base_path, debug, module_name, sengine_path, modules) {
	A_AudioSystem.Setup();
	return await A_ModuleSystem.MainSetup(base_path, debug, module_name, sengine_path, modules);
}

__ALITTLEAPI_ShutdownMainModule = function() {
	A_LayerManager.Shutdown();
	A_AudioSystem.Shutdown();
	A_ModuleSystem.MainShutdown();
}

}