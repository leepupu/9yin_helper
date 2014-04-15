#include "StdAfx.h"
#include <stdio.h>
#include <vector>
#include <iomanip>
#include <string.h>
#include <wininet.h>
#include <windows.h>
#include <sstream>
#include <curl/curl.h>
#ifndef market_incestigator_h
#define market_incestigator_h
class MarketInvestigator
{
public:
	MarketInvestigator();
	static DWORD WINAPI thread_refresh_stalls(void*);
	void on_receive_18(void);
	void on_receive_key(char*);
	void on_receive_value(unsigned int, char*, unsigned int);
	void on_leave_18(void);

	void on_receive_market_list(wchar_t*);//

	void send_query_all_market(void);
	void set_send_packet_func(void*);

	unsigned int monitor;
	unsigned int monitor_opening_stalls;
	bool next_signal;
	std::wstring now_stall_owner;
	

private:
	static DWORD WINAPI thread_open_stalls(void*);
	
	static std::wstring StringToWstring(const std::string);
	void open_a_stall(wchar_t*);
	void store_a_record_to_mongo_db(void);
	std::vector<std::wstring> vec;
	std::wstring mongo_server_store_url;
	bool is_receiving;
	bool is_processing_stalls;
	unsigned int state;
	void (__cdecl *pFuncSendPacket)(char* , unsigned int);
	wchar_t** table;
	wchar_t* stalls_data;
	int split_num;
	
	int stalls_num;
	void (MarketInvestigator::*pFuncOpenStallThread)();
};
#endif