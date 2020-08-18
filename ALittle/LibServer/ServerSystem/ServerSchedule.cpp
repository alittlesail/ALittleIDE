
#include <asio.hpp>

#include "ServerSchedule.h"

#include "ALittle/LibServer/MysqlSystem/MysqlConnection.h"
#include "ALittle/LibServer/HttpSystem/Server/HttpServer.h"
#include "ALittle/LibServer/HttpSystem/Server/HttpSender.h"
#include "ALittle/LibServer/ClientSystem/ClientReceiver.h"

#include "ALittle/LibCommon/Helper/TimeHelper.h"
#include "ALittle/LibCommon/Helper/LogHelper.h"
#include "ALittle/LibCommon/Helper/DumpHelper.h"
#include "ALittle/LibCommon/Helper/StringHelper.h"
#include "ALittle/LibCommon/Helper/HttpHelper.h"
#include "ALittle/LibCommon/Helper/FileHelper.h"
#include "ALittle/LibCommon/Tool/LogSystem.h"

#include "ALittle/LibServer/HttpSystem/Client/HttpClientText.h"
#include "ALittle/LibServer/HttpSystem/Client/HttpClientPost.h"
#include "ALittle/LibServer/MysqlSystem/MysqlConnection.h"
#include "ALittle/LibServer/MysqlSystem/MysqlStatementQuery.h"

namespace ALittle
{

ServerSchedule::ServerSchedule(const std::string& core_path
	, const std::string& std_path
	, const std::string& sengine_path
	, const std::string& module_title
	, const std::string& module_name
	, const std::string& module_path
	, const std::string& config_path)
{
    m_is_exit = false;
	m_core_path = core_path;
	m_std_path = std_path;
	m_sengine_path = sengine_path;
	m_module_title = module_title;
	m_module_path = module_path;
	m_module_name = module_name;
	m_config_path = config_path;
	m_current_time = 0;
	m_route_system = nullptr;
}

ServerSchedule::~ServerSchedule()
{
}

void ServerSchedule::RegisterToScript()
{
	lua_State* L = m_script_system.GetLuaState();

	// register script system
	luabridge::getGlobalNamespace(L)
        .beginClass<ServerSchedule>("__CPPAPIServerSchedule")
        .addFunction("StartMysqlQuery", &ServerSchedule::StartMysqlQuery)
        .addFunction("AddMysqlStatement", &ServerSchedule::AddMysqlStatement)
        .addFunction("AddMysqlNormal", &ServerSchedule::AddMysqlNormal)
		.addFunction("AddMysqlEmpty", &ServerSchedule::AddMysqlEmpty)
		.addFunction("HttpGet", &ServerSchedule::HttpGet)
		.addFunction("HttpPost", &ServerSchedule::HttpPost)
		.addFunction("CreateHttpServer", &ServerSchedule::CreateHttpServer)
		.addFunction("GetHttpServerYunIp", &ServerSchedule::GetHttpServerYunIp)
		.addFunction("GetHttpServerIp", &ServerSchedule::GetHttpServerIp)
		.addFunction("GetHttpServerPort", &ServerSchedule::GetHttpServerPort)
		.addFunction("HttpClose", &ServerSchedule::HttpClose)
		.addFunction("HttpSendString", &ServerSchedule::HttpSendString)
		.addFunction("HttpSendFile", &ServerSchedule::HttpSendFile)
		.addFunction("HttpStartReceiveFile", &ServerSchedule::HttpStartReceiveFile)
		.addFunction("CreateClientServer", &ServerSchedule::CreateClientServer)
		.addFunction("GetClientServerYunIp", &ServerSchedule::GetClientServerYunIp)
		.addFunction("GetClientServerIp", &ServerSchedule::GetClientServerIp)
		.addFunction("GetClientServerPort", &ServerSchedule::GetClientServerPort)
		.addFunction("ClientClose", &ServerSchedule::ClientClose)
		.addFunction("ClientSend", &ServerSchedule::ClientSend)
		.addFunction("StartRouteSystem", &ServerSchedule::StartRouteSystem)
		.addFunction("GetRouteType", &ServerSchedule::GetRouteType)
		.addFunction("GetRouteNum", &ServerSchedule::GetRouteNum)
		.addFunction("GetRouteId", &ServerSchedule::GetRouteId)
		.addFunction("CreateConnectServer", &ServerSchedule::CreateConnectServer)
		.addFunction("CreateConnectClient", &ServerSchedule::CreateConnectClient)
		.addFunction("ConnectSession", &ServerSchedule::ConnectSession)
		.addFunction("CalcConnectKey", &ServerSchedule::CalcConnectKey)
		.addFunction("SessionClose", &ServerSchedule::SessionClose)
		.addFunction("SessionSend", &ServerSchedule::SessionSend)
		.endClass();

	luabridge::setGlobal(L, this, "__CPPAPI_ServerSchedule");

	luabridge::getGlobalNamespace(L)
		.beginClass<MysqlStatementQuery>("__CPPAPIMysqlStatementQuery")
		.addConstructor<void(*)()>()
        .addFunction("SetSQL", &MysqlStatementQuery::SetSQL)
        .addFunction("GetSQL", &MysqlStatementQuery::GetSQL)
		.addFunction("GetCount", &MysqlStatementQuery::GetCount)
		.addFunction("GetAffectCount", &MysqlStatementQuery::GetAffectCount)
		.addFunction("PushBool", &MysqlStatementQuery::PushBool)
		.addFunction("PushInt", &MysqlStatementQuery::PushInt)
		.addFunction("PushDouble", &MysqlStatementQuery::PushDouble)
		.addFunction("PushI64", &MysqlStatementQuery::PushLongLong)
		.addFunction("PushString", &MysqlStatementQuery::PushString)
		.addFunction("HasNext", &MysqlStatementQuery::HasNext)
		.addFunction("Next", &MysqlStatementQuery::Next)
		.addFunction("ReadType", &MysqlStatementQuery::ReadType)
		.addFunction("ReadBool", &MysqlStatementQuery::ReadBool)
		.addFunction("ReadInt", &MysqlStatementQuery::ReadInt)
		.addFunction("ReadDouble", &MysqlStatementQuery::ReadDouble)
		.addFunction("ReadI64", &MysqlStatementQuery::ReadLongLong)
		.addFunction("ReadString", &MysqlStatementQuery::ReadString)
		.endClass();
}

int ServerSchedule::Run()
{
	// ��ʼ���ű�ϵͳ
	m_script_system.Init(m_module_title);
	
	// ����ؽӿ�ע�ᵽ�ű�
	RegisterToScript();

	m_script_system.Require((m_core_path + "Script/ALittle").c_str());
	m_script_system.Invoke("RequireCore", (m_core_path + "Script/").c_str());
	m_script_system.Require((m_std_path + "Script/ALittle").c_str());
	m_script_system.Invoke("RequireStd", (m_std_path + "Script/").c_str());
	m_script_system.Require((m_sengine_path + "Script/ALittle").c_str());
	m_script_system.Invoke("RequireSEngine", (m_sengine_path + "Script/").c_str());

	m_script_system.Invoke("__ALITTLEAPI_SetupMainModule", m_sengine_path.c_str(), m_module_path.c_str(), m_module_name.c_str(), m_config_path.c_str());

	// ������ʱ��
	m_current_time = TimeHelper::GetCurMSTime();
	const int HEARTBEAT = 20;
	m_timer = AsioTimerPtr(new AsioTimer(m_io_service, std::chrono::milliseconds(HEARTBEAT)));
	m_timer->async_wait(std::bind(&ServerSchedule::Update, this, std::placeholders::_1, HEARTBEAT));

	// run
	asio::error_code ec;
	m_io_service.run(ec);
	if (ec) ALITTLE_ERROR("io server run error: " << SUTF8(asio::system_error(ec).what()));

	m_mysql_system.Shutdown();
	m_script_system.Invoke("__ALITTLEAPI_ShutdownMainModule", m_module_name.c_str());

	m_script_system.Release();
	m_file_cache.Shutdown();

	for (auto it = m_http_server_set.begin(); it != m_http_server_set.end(); ++it)
		(*it)->Close();
	m_http_server_set.clear();
	m_id_map_http.clear();

	for (auto it = m_client_server_set.begin(); it != m_client_server_set.end(); ++it)
		(*it)->Close();
	m_client_server_set.clear();
	m_id_map_client.clear();
	m_client_map_id.clear();

	if (m_route_system != nullptr)
	{
		m_route_system->Close();
		delete m_route_system;
		m_route_system = nullptr;
	}

	return 0;
}

void ServerSchedule::Update(const asio::error_code& ec, int interval)
{
	if (m_is_exit) return;

	time_t time = TimeHelper::GetCurMSTime();

	// update script logic
	m_script_system.Invoke("__ALITTLEAPI_Update", time - m_current_time);
	m_current_time = time;

	
	if (m_is_exit) return;
	m_timer->expires_at(std::chrono::system_clock::now() + std::chrono::milliseconds(interval));
	if (m_is_exit) return;
	m_timer->async_wait(std::bind(&ServerSchedule::Update, this, std::placeholders::_1, interval));
}

void ServerSchedule::StartMysqlQuery(int thread_count, const char* ip, const char* username, const char* password, unsigned int port, const char* db_name)
{
	m_mysql_system.Setup(thread_count, ip, username, password, port, db_name);
}

bool ServerSchedule::AddMysqlStatement(int thread_id, int query_id, MysqlStatementQuery* query)
{
	return m_mysql_system.AddTask(thread_id, query_id, query, this);
}

bool ServerSchedule::AddMysqlNormal(int thread_id, int query_id, const char* sql)
{
	return m_mysql_system.AddTask(thread_id, query_id, sql, this);
}

bool ServerSchedule::AddMysqlEmpty(int thread_id, int query_id)
{
	return m_mysql_system.AddTask(thread_id, query_id, this);
}

void ServerSchedule::HandleMysqlQuery(int query_id, bool result, std::string reason)
{
	if (result)
		m_script_system.Invoke("__ALITTLEAPI_HandleMysqlQueryTask", query_id);
	else
		m_script_system.Invoke("__ALITTLEAPI_HandleMysqlQueryTask", query_id, reason.c_str());
}

void ServerSchedule::HandleMysqlEmpty(int query_id, bool result, std::string reason)
{
	if (result)
		m_script_system.Invoke("__ALITTLEAPI_HandleMysqlEmptyTask", query_id);
	else
		m_script_system.Invoke("__ALITTLEAPI_HandleMysqlEmptyTask", query_id, reason.c_str());
}


bool ServerSchedule::CreateHttpServer(const char* yun_ip, const char* ip, int port, bool is_ssl)
{
	std::string yun_ip_str;
	if (yun_ip != nullptr) yun_ip_str = yun_ip;
	std::string ip_str;
	if (ip != nullptr) ip_str = ip;

	HttpServerPtr server = HttpServerPtr(new HttpServer);
	bool result = server->Start(yun_ip_str, ip_str, port, 30, is_ssl, this, "", "", "");
	if (!result) return false;

	m_http_server_set.insert(server);
	return true;
}

const char* ServerSchedule::GetHttpServerYunIp() const
{
	if (m_http_server_set.empty()) return nullptr;

	auto it = m_http_server_set.begin();
	return (*it)->GetYunIp().c_str();
}

const char* ServerSchedule::GetHttpServerIp() const
{
	if (m_http_server_set.empty()) return nullptr;

	auto it = m_http_server_set.begin();
	return (*it)->GetIp().c_str();
}

int ServerSchedule::GetHttpServerPort() const
{
	if (m_http_server_set.empty()) return 0;

	auto it = m_http_server_set.begin();
	return (*it)->GetPort();
}

void ServerSchedule::HandleHttpMessage(HttpSenderPtr sender, const std::string& msg)
{
	int id = m_id_creator.CreateID();
	m_id_map_http[id] = sender;
	sender->m_id = id;

	std::string method;
	std::string path;
	std::string param;
	std::string content_type;
	std::string content;
	if (!HttpHelper::AnalysisRequest(msg, method, path, &param, &content_type, &content))
	{
		ALITTLE_WARN("error request:\n" << msg);
		sender->Close();
		return;
	}

	if (path.size() && path[0] == '/')
		path = path.substr(1);

	m_script_system.Invoke("__ALITTLEAPI_HttpTask", id, path.c_str(), param.c_str(), content.c_str());
}

bool ServerSchedule::HandleHttpFileMessage(HttpSenderPtr sender, const std::string& msg)
{
	std::string method;
	std::string path;
	std::string param;
	std::string content_type;
	std::string content;
	if (!HttpHelper::AnalysisRequest(msg, method, path, &param, &content_type, &content))
	{
		ALITTLE_WARN("error request:\n" << msg);
		return false;
	}

	int id = m_id_creator.CreateID();
	m_id_map_http[id] = sender;
	sender->m_id = id;

	if (path.size() && path[0] == '/')
		path = path.substr(1);

	m_script_system.Invoke("__ALITTLEAPI_HttpFileTask", id, path.c_str(), param.c_str(), content.c_str());
	return true;
}

void ServerSchedule::HttpGet(int id, const char* url)
{
	HttpClientTextPtr client = HttpClientTextPtr(new HttpClientText);
	client->SendRequest(url, true, "text/html", "", 0
		, [this, id](bool result, const std::string& body, const std::string& head)
		{
			if (result)
				m_script_system.Invoke("__ALITTLEAPI_HttpSucceed", id, body.c_str());
			else
				m_script_system.Invoke("__ALITTLEAPI_HttpFailed", id, head.c_str());
		}, &m_io_service, "", "");
}

void ServerSchedule::HttpPost(int id, const char* url, const char* type, const char* content)
{
	HttpClientTextPtr client = HttpClientTextPtr(new HttpClientText);
	client->SendRequest(url, false, type, content, strlen(content)
		, [this, id](bool result, const std::string& body, const std::string& head)
		{
			if (result)
				m_script_system.Invoke("__ALITTLEAPI_HttpSucceed", id, body.c_str());
			else
				m_script_system.Invoke("__ALITTLEAPI_HttpFailed", id, head.c_str());
		}, &m_io_service, "", "");
}

void ServerSchedule::HttpClose(int id)
{
	auto it = m_id_map_http.find(id);
	if (it == m_id_map_http.end())
		return;

	HttpSenderPtr sender = it->second.lock();
	m_id_creator.ReleaseID(id);
	m_id_map_http.erase(it);
	if (!sender) return;

	sender->Close();
}

void ServerSchedule::HttpSendString(int id, const char* content)
{
	auto it = m_id_map_http.find(id);
	if (it == m_id_map_http.end())
		return;

	HttpSenderPtr sender = it->second.lock();
	m_id_creator.ReleaseID(id);
	m_id_map_http.erase(it);
	if (!sender) return;

	sender->SendString(content);
}

void ServerSchedule::HttpSendFile(int id, const char* file_path, int start_size)
{
	auto it = m_id_map_http.find(id);
	if (it == m_id_map_http.end())
		return;

	HttpSenderPtr sender = it->second.lock();
	m_id_creator.ReleaseID(id);
	m_id_map_http.erase(it);
	if (!sender) return;

	std::string content_type = HttpHelper::GetContentTypeByExt(FileHelper::GetFileExtByPath(file_path));
	std::string show_name = FileHelper::GetFileNameByPath(file_path);
	sender->SendFile(file_path, content_type.c_str(), false, start_size, true, show_name.c_str());
}

void ServerSchedule::HttpStartReceiveFile(int id, const char* file_path, int start_size)
{
	auto it = m_id_map_http.find(id);
	if (it == m_id_map_http.end()) return;

	HttpSenderPtr sender = it->second.lock();
	if (!sender)
	{
		HandleHttpFileCompletedMessage(sender, std::string(file_path), false, std::string(u8"Http�����Ѿ��Ͽ�"));
		return;
	}

	std::string reason;
	if (!sender->StartReceiveFile(file_path, start_size, reason))
	{
		HandleHttpFileCompletedMessage(sender, std::string(file_path), false, reason);
	}
}

void ServerSchedule::HandleHttpFileCompletedMessage(HttpSenderPtr sender, const std::string& file_path, bool succeed, const std::string& reason)
{
	// ���ﱣ֤luaЭ��ִ�����ϸ��˳��
	m_io_service.post(std::bind(&ServerSchedule::HandleHttpFileCompletedMessageImpl, this, sender, std::string(file_path), succeed, reason));
}

void ServerSchedule::HandleHttpFileCompletedMessageImpl(HttpSenderPtr sender, const std::string& file_path, bool succeed, const std::string& reason)
{
	int id = sender->m_id;

	auto it = m_id_map_http.find(id);
	if (it == m_id_map_http.end())
		return;

	if (succeed)
		m_script_system.Invoke("__ALITTLEAPI_HttpFileCompletedTask", id);
	else
		m_script_system.Invoke("__ALITTLEAPI_HttpFileCompletedTask", id, reason.c_str());
}

bool ServerSchedule::CreateClientServer(const char* yun_ip, const char* ip, int port)
{
	std::string yun_ip_str;
	if (yun_ip != nullptr) yun_ip_str = yun_ip;
	std::string ip_str;
	if (ip != nullptr) ip_str = ip;
	ClientServerPtr server = ClientServerPtr(new ClientServer());
	if (!server->Start(yun_ip_str, ip_str, port, 30, this)) return false;
	m_client_server_set.insert(server);
	return true;
}

const char* ServerSchedule::GetClientServerYunIp() const
{
	if (m_client_server_set.empty()) return nullptr;

	auto it = m_client_server_set.begin();
	return (*it)->GetYunIp().c_str();
}

const char* ServerSchedule::GetClientServerIp() const
{
	if (m_client_server_set.empty()) return nullptr;

	auto it = m_client_server_set.begin();
	return (*it)->GetIp().c_str();
}

int ServerSchedule::GetClientServerPort() const
{
	if (m_client_server_set.empty()) return 0;

	auto it = m_client_server_set.begin();
	return (*it)->GetPort();
}

void ServerSchedule::HandleClientConnect(ClientReceiverPtr sender)
{
	int id = m_id_creator.CreateID();
	m_id_map_client[id] = sender;
	m_client_map_id[sender] = id;

	m_script_system.Invoke("__ALITTLEAPI_ClientConnect", id, sender->GetRemoteIP().c_str(), sender->GetRemotePort());
}

void ServerSchedule::HandleClientDisconnect(ClientReceiverPtr sender)
{
	auto it = m_client_map_id.find(sender);
	if (it == m_client_map_id.end()) return;
	int id = it->second;
	m_client_map_id.erase(it);

	m_id_map_client.erase(id);
	m_id_creator.ReleaseID(id);

	m_script_system.Invoke("__ALITTLEAPI_ClientDisconnect", id);
}

void ServerSchedule::HandleClientMessage(ClientReceiverPtr sender, int message_size, int message_id, int message_rpcid, void* memory)
{
	auto it = m_client_map_id.find(sender);
	if (it == m_client_map_id.end()) return;

	m_read_factory.Deserialize(memory, message_size);
	m_read_factory.SetID(message_id);
	m_read_factory.SetRpcID(message_rpcid);

	m_script_system.Invoke("__ALITTLEAPI_ClientMessage", it->second, message_id, message_rpcid, &m_read_factory);
}

void ServerSchedule::ClientClose(int id)
{
	auto it = m_id_map_client.find(id);
	if (it == m_id_map_client.end()) return;

	it->second->Close();
	m_id_map_client.erase(it);
}

void ServerSchedule::ClientSend(int id, MessageWriteFactory* factory)
{
	auto it = m_id_map_client.find(id);
	if (it == m_id_map_client.end()) return;

	it->second->Send(*factory);
}

void ServerSchedule::StartRouteSystem(int route_type, int route_num)
{
	if (m_route_system != nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem �Ѿ���ʼ��");
		return;
	}

	ROUTE_ID route_id = RouteIdDefine::CalcRouteId(static_cast<unsigned char>(route_type)
		, static_cast<unsigned char>(route_num));
	m_route_system = new RouteSystem();
	m_route_system->Start(route_id, 30, 5, this);
}

int ServerSchedule::GetRouteType()
{
	if (m_route_system == nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		return 0;
	}

	return RouteIdDefine::CalcRouteType(m_route_system->GetRouteId());
}

int ServerSchedule::GetRouteNum()
{
	if (m_route_system == nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		return 0;
	}

	return RouteIdDefine::CalcRouteNum(m_route_system->GetRouteId());
}

int ServerSchedule::GetRouteId()
{
	if (m_route_system == nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		return 0;
	}

	return m_route_system->GetRouteId();
}


bool ServerSchedule::CreateConnectServer(const char* yun_ip, const char* ip, int port)
{
	if (m_route_system == nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		return false;
	}

	std::string yun_ip_str;
	if (yun_ip != nullptr) yun_ip_str = yun_ip;
	std::string ip_str;
	if (ip != nullptr) ip_str = ip;
	return m_route_system->CreateConnectServer(yun_ip_str, ip_str, port);
}

bool ServerSchedule::CreateConnectClient(const char* ip, int port)
{
	if (m_route_system == nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		return false;
	}

	std::string ip_str;
	if (ip != nullptr) ip_str = ip;
	return m_route_system->CreateConnectClient(ip_str, port);
}

void ServerSchedule::ConnectSession(int route_type, int route_num)
{
	if (m_route_system == nullptr)
	{
		ALITTLE_ERROR(u8"RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		m_script_system.Invoke("__ALITTLEAPI_ConnectSessionFailed", route_type, route_num, "RouteSystem��δ��ʼ���������StartRouteSystem���г�ʼ��");
		return;
	}

	ROUTE_ID route_id = RouteIdDefine::CalcRouteId(static_cast<unsigned char>(route_type), static_cast<unsigned char>(route_num));
	m_route_system->ConnectRoute(route_id, [this, route_type, route_num](const std::string* reason, SessionConnectionPtr session)
		{
			if (reason != nullptr)
			{
				m_script_system.Invoke("__ALITTLEAPI_ConnectSessionFailed", route_type, route_num, reason->c_str());
				return;
			}
			m_script_system.Invoke("__ALITTLEAPI_ConnectSessionSucceed", session->GetConnectKey(), route_type, route_num);
		});
}

int ServerSchedule::CalcConnectKey(int src_route_type, int src_route_num, int dst_route_type, int dst_route_num)
{
	return RouteIdDefine::CalcConnectKey(RouteIdDefine::CalcRouteId(src_route_type, src_route_num), RouteIdDefine::CalcRouteId(dst_route_type, dst_route_num));
}

void ServerSchedule::HandleSessionNotify(bool is_connected, SessionConnectionPtr session)
{
	if (is_connected)
	{
		m_session_map[session->GetConnectKey()] = session;
		m_script_system.Invoke("__ALITTLEAPI_SessionConnect", session->GetConnectKey(), session->GetRouteType(), session->GetRouteNum());
	}
	else
	{
		m_session_map.erase(session->GetConnectKey());
		m_script_system.Invoke("__ALITTLEAPI_SessionDisconnect", session->GetConnectKey(), session->GetRouteType(), session->GetRouteNum());
	}
}

void ServerSchedule::HandleSessionMessage(CONNECT_KEY connect_key, int message_id, int message_rpcid, void* message_body, int message_size)
{
	m_read_factory.Deserialize(message_body, message_size);
	m_read_factory.SetID(message_id);
	m_read_factory.SetRpcID(message_rpcid);

	m_script_system.Invoke("__ALITTLEAPI_SessionMessage", connect_key, message_id, message_rpcid, &m_read_factory);
}

void ServerSchedule::SessionClose(CONNECT_KEY connect_key)
{
	auto it = m_session_map.find(connect_key);
	if (it == m_session_map.end()) return;

	m_route_system->CloseConnect(it->second);
	m_session_map.erase(it);
}

void ServerSchedule::SessionSend(CONNECT_KEY connect_key, MessageWriteFactory* factory)
{
	auto it = m_session_map.find(connect_key);
	if (it == m_session_map.end()) return;

	it->second->Send(*factory);
}

void ServerSchedule::Exit()
{
	m_is_exit = true;
	if (m_timer)
	{
		asio::error_code ec;
		m_timer->cancel(ec);
	}
	m_io_service.stop();
}

void ServerSchedule::Execute(std::function<void()> func)
{
	m_io_service.post(func);
}

void ServerSchedule::HandleConsoleCmd(std::string cmd)
{
	m_script_system.Invoke("__ALITTLEAPI_HandleConsoleCmd", cmd.c_str());
}

} // ALittle