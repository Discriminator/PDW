#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>

#include <deque>
#include <string>

#include "http_post.h"
#include "../utils/debug.h"

#include <nlohmann/json.hpp>

#pragma comment(lib, "winhttp.lib")

#define HTTPPOST_DEFAULT_QUEUE_MAX 100
#define HTTPPOST_DEFAULT_TTL_SEC   300
#define HTTPPOST_MAX_QUEUE_HARD    5000

typedef struct _HTTPPOSTCONFIG
{
	int enabled;
	int authEnabled;
	int queueMax;
	int queueTtlSeconds;
	std::string url;
	std::string user;
	std::string password;
} HTTPPOSTCONFIG;

typedef struct _HTTPPOSTENTRY
{
	ULONGLONG enqueueTick;
	std::string payload;
} HTTPPOSTENTRY;

static HANDLE g_httpThread = 0;
static HANDLE g_httpEvent = 0;
static BOOL g_keepbusy = FALSE;
static BOOL g_httpStateInit = FALSE;
static CRITICAL_SECTION g_httpLock;
static HTTPPOSTCONFIG g_httpConfig;
static std::deque<HTTPPOSTENTRY> g_httpQueue;

static int ClampQueueMax(int queueMax)
{
	if (queueMax < 1) return HTTPPOST_DEFAULT_QUEUE_MAX;
	if (queueMax > HTTPPOST_MAX_QUEUE_HARD) return HTTPPOST_MAX_QUEUE_HARD;
	return queueMax;
}

static int ClampTtl(int ttl)
{
	if (ttl < 1) return HTTPPOST_DEFAULT_TTL_SEC;
	if (ttl > 86400) return 86400;
	return ttl;
}

static std::wstring ToWide(const char *input)
{
	if (!input || !input[0]) return L"";
	int needed = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
	UINT codePage = CP_UTF8;
	if (needed <= 0)
	{
		codePage = CP_ACP;
		needed = MultiByteToWideChar(codePage, 0, input, -1, NULL, 0);
	}
	if (needed <= 0) return L"";

	std::wstring out;
	out.resize(needed);
	if (!out.empty())
	{
		int ok = MultiByteToWideChar(codePage, 0, input, -1, &out[0], needed);
		if (ok <= 0) return L"";
		if (!out.empty() && out[out.size() - 1] == L'\0') out.resize(out.size() - 1);
	}
	return out;
}

static BOOL BuildBasicAuthHeader(const std::string& user, const std::string& password, std::wstring& outHeader)
{
	std::string combined = user;
	combined += ":";
	combined += password;

	DWORD outLen = 0;
	if (!CryptBinaryToStringA((const BYTE*)combined.c_str(), (DWORD)combined.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &outLen))
	{
		return FALSE;
	}

	std::string encoded;
	encoded.resize(outLen + 2);
	if (!CryptBinaryToStringA((const BYTE*)combined.c_str(), (DWORD)combined.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &encoded[0], &outLen))
	{
		return FALSE;
	}
	encoded.resize(outLen);

	std::string header = "Authorization: Basic ";
	header += encoded;
	header += "\r\n";
	outHeader = ToWide(header.c_str());
	return TRUE;
}

static std::string ConvertToUtf8(const char *src, UINT codePage)
{
	if (!src || !src[0]) return "";

	int wideLen = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, src, -1, NULL, 0);
	if (wideLen <= 0) return "";

	std::wstring wide;
	wide.resize(wideLen);

	if (MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, src, -1, &wide[0], wideLen) <= 0)
	{
		return "";
	}

	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8Len <= 0) return "";

	std::string utf8;
	utf8.resize(utf8Len - 1);

	if (WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &utf8[0], utf8Len, NULL, NULL) <= 0)
	{
		return "";
	}

	return utf8;
}

static std::string ToUtf8Auto(const char *src)
{
	std::string utf8 = ConvertToUtf8(src, CP_UTF8);
	if (!utf8.empty() || !src || !src[0])
	{
		return utf8;
	}

	return ConvertToUtf8(src, CP_ACP);
}

static std::string JsonEscapeUtf8(const char *src)
{
	std::string utf8 = ToUtf8Auto(src);
	std::string out;

	for (size_t i = 0; i < utf8.size(); ++i)
	{
		unsigned char ch = (unsigned char)utf8[i];
		switch (ch)
		{
			case '\"': out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\b': out += "\\b";  break;
			case '\f': out += "\\f";  break;
			case '\n': out += "\\n";  break;
			case '\r': out += "\\r";  break;
			case '\t': out += "\\t";  break;
			default:
				if (ch < 0x20)
				{
					char tmp[7];
					wsprintfA(tmp, "\\u%04X", ch);
					out += tmp;
				}
				else
				{
					out += (char)ch;
				}
				break;
		}
	}
	return out;
}

static BOOL SendJsonPost(const HTTPPOSTCONFIG& cfg, const std::string& payload)
{
	if (!cfg.enabled || cfg.url.empty()) return FALSE;

	std::wstring wurl = ToWide(cfg.url.c_str());
	if (wurl.empty()) return FALSE;

	URL_COMPONENTS uc;
	ZeroMemory(&uc, sizeof(uc));
	uc.dwStructSize = sizeof(uc);

	wchar_t hostName[512] = { 0 };
	wchar_t urlPath[2048] = { 0 };
	wchar_t extraInfo[2048] = { 0 };
	uc.lpszHostName = hostName;
	uc.dwHostNameLength = (DWORD)(sizeof(hostName) / sizeof(hostName[0]));
	uc.lpszUrlPath = urlPath;
	uc.dwUrlPathLength = (DWORD)(sizeof(urlPath) / sizeof(urlPath[0]));
	uc.lpszExtraInfo = extraInfo;
	uc.dwExtraInfoLength = (DWORD)(sizeof(extraInfo) / sizeof(extraInfo[0]));

	if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc))
	{
		return FALSE;
	}

	std::wstring whost(hostName, uc.dwHostNameLength);
	std::wstring wpath(urlPath, uc.dwUrlPathLength);
	wpath += std::wstring(extraInfo, uc.dwExtraInfoLength);
	if (wpath.empty()) wpath = L"/";

	HINTERNET hSession = WinHttpOpen(L"PDW-HTTPPOST/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) return FALSE;

	HINTERNET hConnect = WinHttpConnect(hSession, whost.c_str(), uc.nPort, 0);
	if (!hConnect)
	{
		WinHttpCloseHandle(hSession);
		return FALSE;
	}

	DWORD dwOpenFlags = (uc.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wpath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwOpenFlags);
	if (!hRequest)
	{
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return FALSE;
	}

	WinHttpSetTimeouts(hRequest, 5000, 5000, 10000, 15000);

	std::wstring headers = L"Content-Type: application/json\r\n";
	if (cfg.authEnabled)
	{
		std::wstring authHeader;
		if (BuildBasicAuthHeader(cfg.user, cfg.password, authHeader))
		{
			headers += authHeader;
		}
	}

	BOOL sent = WinHttpSendRequest(hRequest,
		headers.c_str(),
		(DWORD)-1L,
		(LPVOID)payload.c_str(),
		(DWORD)payload.size(),
		(DWORD)payload.size(),
		0);

	if (!sent || !WinHttpReceiveResponse(hRequest, NULL))
	{
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return FALSE;
	}

	DWORD statusCode = 0;
	DWORD statusSize = sizeof(statusCode);
	BOOL gotStatus = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	if (!gotStatus) return FALSE;
	return (statusCode >= 200 && statusCode < 300);
}

static DWORD WINAPI HttpPostThreadFunc(LPVOID)
{
	while (g_keepbusy)
	{
		HTTPPOSTENTRY item;
		HTTPPOSTCONFIG cfg;
		BOOL hasItem = FALSE;

		EnterCriticalSection(&g_httpLock);
		if (!g_httpQueue.empty())
		{
			item = g_httpQueue.front();
			g_httpQueue.pop_front();
			hasItem = TRUE;
		}
		cfg = g_httpConfig;
		LeaveCriticalSection(&g_httpLock);

		if (!hasItem)
		{
			WaitForSingleObject(g_httpEvent, 1000);
			ResetEvent(g_httpEvent);
			continue;
		}

		ULONGLONG ageMs = GetTickCount64() - item.enqueueTick;
		ULONGLONG ttlMs = (ULONGLONG)cfg.queueTtlSeconds * 1000ULL;
		if (ageMs > ttlMs)
		{
			OUTPUTDEBUGMSG((("HTTP POST queue: dropping expired item (TTL exceeded)")));
			continue;
		}

		if (!SendJsonPost(cfg, item.payload))
		{
			EnterCriticalSection(&g_httpLock);
			g_httpQueue.push_front(item);
			LeaveCriticalSection(&g_httpLock);
			Sleep(1000);
		}
	}
	return 0;
}

static void EnsureHttpState()
{
	if (g_httpStateInit) return;
	InitializeCriticalSection(&g_httpLock);
	g_httpEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_httpConfig.enabled = 0;
	g_httpConfig.authEnabled = 0;
	g_httpConfig.queueMax = HTTPPOST_DEFAULT_QUEUE_MAX;
	g_httpConfig.queueTtlSeconds = HTTPPOST_DEFAULT_TTL_SEC;
	g_httpStateInit = TRUE;
}

int HttpPostInit(int enabled, const char *url, int authEnabled, const char *user, const char *password, int queueMax, int queueTtlSeconds)
{
	EnsureHttpState();

	HTTPPOSTCONFIG cfg;
	cfg.enabled = enabled ? 1 : 0;
	cfg.authEnabled = authEnabled ? 1 : 0;
	cfg.queueMax = ClampQueueMax(queueMax);
	cfg.queueTtlSeconds = ClampTtl(queueTtlSeconds);
	cfg.url = url ? url : "";
	cfg.user = user ? user : "";
	cfg.password = password ? password : "";

	BOOL shouldRun = (cfg.enabled && !cfg.url.empty()) ? TRUE : FALSE;

	EnterCriticalSection(&g_httpLock);
	g_httpConfig = cfg;
	LeaveCriticalSection(&g_httpLock);

	if (shouldRun)
	{
		if (!g_httpThread)
		{
			DWORD threadId = 0;
			g_keepbusy = TRUE;
			g_httpThread = CreateThread(0, 0, HttpPostThreadFunc, 0, 0, &threadId);
		}
		SetEvent(g_httpEvent);
	}
	else
	{
		if (g_httpThread)
		{
			g_keepbusy = FALSE;
			SetEvent(g_httpEvent);
			WaitForSingleObject(g_httpThread, 3000);
			CloseHandle(g_httpThread);
			g_httpThread = 0;
		}

		EnterCriticalSection(&g_httpLock);
		g_httpQueue.clear();
		LeaveCriticalSection(&g_httpLock);
	}
	return 0;
}

void HttpPostShutdown()
{
	HttpPostInit(0, "", 0, "", "", HTTPPOST_DEFAULT_QUEUE_MAX, HTTPPOST_DEFAULT_TTL_SEC);
}

int HttpPostQueueMessage(int bMatch, int bMonitorOnly,
	const char *sz1, const char *sz2, const char *sz3, const char *sz4,
	const char *sz5, const char *sz6, const char *sz7, const char *szLabel)
{
	EnsureHttpState();

	try
	{
		HTTPPOSTCONFIG cfg;
		EnterCriticalSection(&g_httpLock);
		cfg = g_httpConfig;
		LeaveCriticalSection(&g_httpLock);

		if (!cfg.enabled || cfg.url.empty()) return 0;

		nlohmann::json j;
		j["address"] = sz1 ? sz1 : "";
		j["time"] = sz2 ? sz2 : "";
		j["date"] = sz3 ? sz3 : "";
		j["mode"] = sz4 ? sz4 : "";
		j["type"] = sz5 ? sz5 : "";
		j["bitrate"] = sz6 ? sz6 : "";
		j["message"] = sz7 ? sz7 : "";
		j["label"] = szLabel ? szLabel : "";
		j["match"] = (bMatch != 0);
		j["monitor_only"] = (bMonitorOnly != 0);

		const std::string json = j.dump();

		HTTPPOSTENTRY entry;
		entry.enqueueTick = GetTickCount64();
		entry.payload = json;

		EnterCriticalSection(&g_httpLock);
		if (g_httpQueue.size() >= (size_t)cfg.queueMax)
		{
			g_httpQueue.pop_front();
			OUTPUTDEBUGMSG((("HTTP POST queue: queue full, dropping oldest item")));
		}
		g_httpQueue.push_back(entry);
		LeaveCriticalSection(&g_httpLock);

		SetEvent(g_httpEvent);
		return 0;
	}
	catch (const std::exception& e)
	{
		OUTPUTDEBUGMSG(("HTTP POST exception: "));
		OUTPUTDEBUGMSG((e.what()));
		return 0;
	}
	catch (...)
	{
		OUTPUTDEBUGMSG(("HTTP POST unknown exception"));
		return 0;
	}
}
