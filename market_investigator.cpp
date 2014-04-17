#include "stdafx.h"
#include "market_investigator.h"

using namespace std;

const int MarketInvestigator::MARKET = 1;
const int MarketInvestigator::SHOP = 2;
const int MarketInvestigator::NOTHING = 0;

MarketInvestigator::MarketInvestigator()
{
	is_receiving=false;
	next_signal = false;
	monitor_opening_stalls = 0;
	monitor=0;
	mongo_server_store_url = L"http://127.0.0.1/a.php";
}

std::wstring MarketInvestigator::StringToWstring(const std::string str)
{
    unsigned len = str.size() * 2;
    setlocale(LC_CTYPE, "");     
    wchar_t *p = new wchar_t[len];
    mbstowcs(p,str.c_str(),len);
    std::wstring str1(p);
    delete[] p;
    return str1;
}

void MarketInvestigator::on_receive_18(void)
{
	//printf("receive 18\n");
	monitor = 0;
	is_receiving = true;
}

void MarketInvestigator::on_receive_key(char* name)
{
	if(!is_receiving)
		return;
	wstring ws = StringToWstring(string(name));
	vec.push_back(ws);
	//printf("get key: %s\n", name);
}

void MarketInvestigator::on_receive_value(unsigned int type, char* pData, unsigned int offset)
{
	char the_char;
	short the_short;
	int the_int;
	float the_float;
	double the_double;
	char* the_string;
	__int64 the_long_long;
	wchar_t* the_wstring;
	string s;
	stringstream ss(s);
	char long_long_int_string[1000]={0};

	int len;

	if( !is_receiving || type<0 || type >7 )
		return;

	switch(type)
	{
	case 0:// get char
		the_char = *(pData+offset);
		ss << (int)the_char;
		break;
	case 1:// short
		the_short = *((short*)(pData+offset));
		ss << the_short;
		break;
	case 2:// int
		the_int = *((int*)(pData+offset));
		ss << the_int;
		break;
	case 3:// long long int
		the_long_long = *((__int64*)(pData+offset));
		sprintf(long_long_int_string,"%lld",the_long_long);
		ss << long_long_int_string;
	case 4:// float
		the_float = *((float*)(pData+offset));
		ss << fixed << the_float;
		break;
	case 5:// double
		the_double = *((double*)(pData+offset));
		ss << fixed << the_double;
		break;
	case 6:
		len = *((int*)(pData+offset));
		the_string = new char[len+2];
		memcpy(the_string, pData+offset+4, len);
		ss << the_string;
		break;
	case 7:
		len = *((int*)(pData+offset));
		the_wstring = new wchar_t[len/2];
		memcpy(the_wstring, pData+offset+4, len);
		vec.push_back(wstring(the_wstring));
		break;
	}
	if(type != 7)
		vec.push_back(StringToWstring(ss.str()));

	wstring sss = vec.back();
	DWORD ws;
	//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),sss.c_str(),wcslen((unsigned short*)sss.c_str()),&ws,NULL);

}

void MarketInvestigator::send_query_all_market(void)
{
	char packet[0x30]={0};
	//stuff data
	packet[0] = 0x0A; //packet head
	packet[0x15] = 0x02; //unknow
	packet[0x17] = 0x02; //type: 4 byte data
	*((unsigned int*)(packet+0x18)) = 0x3E0;
	packet[0x1C] = 0x02;
	*((unsigned int*)(packet+0x1D)) = 0x002;
	packet[0x21] = 0xEE;
	packet[0x22] = 0xEE;
	pFuncSendPacket(packet,0x23);
}

void MarketInvestigator::send_query_all_shop(void)
{
	char packet[0x30]={0};
	//stuff data
	packet[0] = 0x0A; //packet head
	packet[0x15] = 0x02; //unknow
	packet[0x17] = 0x02; //type: 4 byte data
	*((unsigned int*)(packet+0x18)) = 0x3E0;
	packet[0x1C] = 0x02;
	*((unsigned int*)(packet+0x1D)) = 0x005;
	packet[0x21] = 0xEE;
	packet[0x22] = 0xEE;
	pFuncSendPacket(packet,0x23);
}

void MarketInvestigator::open_a_shop(wchar_t* shop_id)
{
	char asc_shop_id[1024]={0};
	int shop_id_len = WideCharToMultiByte(CP_ACP, 0, shop_id, -1, asc_shop_id, 1024, NULL, NULL);
	shop_id_len++; // for null end

	char packet[0x70]={0};
	//stuff data
	packet[0] = 0x0A; //packet head
	packet[0x15] = 0x03; //unknow
	packet[0x17] = 0x02; //type: 4 byte data
	*((unsigned int*)(packet+0x18)) = 0x3E0;
	packet[0x1C] = 0x02;
	*((unsigned int*)(packet+0x1D)) = 0x006;

	packet[0x21] = 0x06; // stall owner name
	*((unsigned int*)(packet+0x22)) = shop_id_len;
	memcpy(packet+0x26, asc_shop_id, shop_id_len);//no end symbolic

	packet[0x26+shop_id_len] = 0xEE;
	packet[0x26+shop_id_len+1] = 0xEE;
	pFuncSendPacket(packet,0x26+shop_id_len+2);
}

void MarketInvestigator::open_a_stall(wchar_t* owner)
{
	unsigned int len = wcslen(owner);

	char packet[0x70]={0};
	//stuff data
	packet[0] = 0x0A; //packet head
	packet[0x15] = 0x03; //unknow
	packet[0x17] = 0x02; //type: 4 byte data
	*((unsigned int*)(packet+0x18)) = 0x3E0;
	packet[0x1C] = 0x02;
	*((unsigned int*)(packet+0x1D)) = 0x003;

	packet[0x21] = 0x07; // stall owner name
	*((unsigned int*)(packet+0x22)) = len*2+2;
	memcpy(packet+0x26, owner, len*2);//no end symbolic

	packet[0x26+len*2]=0x0;  // End of wstr
	packet[0x26+len*2+1]=0x0;// End of wstr
	packet[0x26+len*2+2] = 0xEE;
	packet[0x26+len*2+3] = 0xEE;
	pFuncSendPacket(packet,0x26+len*2+4);
}

DWORD WINAPI MarketInvestigator::thread_refresh_stalls(void* Param)
{
	MarketInvestigator* This = (MarketInvestigator*) Param;
	int count = 0;
	do
	{
		printf("refreshing: %d\n", count++);
		This->get_data_for = MarketInvestigator::MARKET;
		This->send_query_all_market();
		This->monitor_opening_stalls = 0;
		
		do
		{
			
			This->monitor_opening_stalls++;
			Sleep(1000);
		}while(This->monitor_opening_stalls<60);
		if(count%10 != 0)
			continue;
		printf("query all shops\n");

		This->send_query_all_shop();
		This->monitor_opening_stalls = 0;
		This->get_data_for = MarketInvestigator::SHOP;
		do
		{
			
			This->monitor_opening_stalls++;
			Sleep(1000);
		}while(This->monitor_opening_stalls<60);
		
		printf("finish browse shops\n");
	}while(true);
	return 0;
}

DWORD WINAPI MarketInvestigator::thread_open_shops(void* Param)
{
	DWORD ws;
	MarketInvestigator* This = (MarketInvestigator*) Param;
	This->stalls_num = This->split_num / 5;
	printf("shops_num: %d\n", This->stalls_num);
	printf("split_num: %d\n", This->split_num);
	for(int i=0,scount=0;(i+1) < This->split_num;i+=5,scount++)
	{
		printf("real i:%d\n", i);
		This->now_stall_owner = wstring(This->table[i+1]);
		printf("open shop: NO. %d:   ", scount+1);
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),This->table[i+1],wcslen((unsigned short*)This->table[i+1]),&ws,NULL);
		printf("\nand waiting...\n");
		This->open_a_shop(This->table[i]);
		This->monitor = 0;
		This->monitor_opening_stalls = 0;
		do
		{
			This->monitor++;
			Sleep(1000);
		}while(This->monitor<2);

		/*
		while(!This->next_signal) // manual type next
			Sleep(3000);
		This->next_signal = 0;*/
	}
	printf("clean data...");
	delete [] This->table;
	delete This->stalls_data;
	printf("OK\n");
	return 0;
}

DWORD WINAPI MarketInvestigator::thread_open_stalls(void* Param)
{
	DWORD ws;
	MarketInvestigator* This = (MarketInvestigator*) Param;
	This->stalls_num = This->split_num / 6;
	printf("stalls_num: %d\n", This->stalls_num);
	for(int i=0,scount=0;i < This->split_num;i+=6,scount++)
	{
		This->now_stall_owner = wstring(This->table[i]);
		printf("open stall: NO. %d:   ", scount+1);
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),This->table[i],wcslen((unsigned short*)This->table[i]),&ws,NULL);
		printf("\nand waiting...\n");
		This->open_a_stall(This->table[i]);
		This->monitor = 0;
		This->monitor_opening_stalls = 0;
		do
		{
			This->monitor++;
			Sleep(1000);
		}while(This->monitor<2);

		/*
		while(!This->next_signal) // manual type next
			Sleep(3000);
		This->next_signal = 0;*/
	}
	printf("clean data...");
	delete [] This->table;
	delete This->stalls_data;
	printf("OK\n");
	return 0;
}

void MarketInvestigator::on_receive_market_list(wchar_t* str)
{
	DWORD ws;
	//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),str,wcslen((unsigned short*)str),&ws,NULL);//to print unicode
	if(get_data_for == MarketInvestigator::NOTHING)
		return;
	int len = wcslen(str);
	stalls_data = new wchar_t[len+2];
	memcpy(stalls_data, str, len*2);
	stalls_data[len] = 0;
	stalls_data[len+1] = 0;
	wchar_t zz = 0;
	table = new wchar_t*[len];
	split_num=0;
	wchar_t* pNow = stalls_data;
	for(int i=0;i<len;i++)
	{
		wchar_t s = stalls_data[i];
		if(s == L'^')
		{
			stalls_data[i] = zz;
			table[split_num++] = pNow;
			pNow = stalls_data+i+1;
		}
	}
	table[split_num++] = pNow;
	/*
	for(i=0;i<split_num;i++)
	{
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),table[i],wcslen((unsigned short*)table[i]),&ws,NULL);
	}
	*/
	if(get_data_for == MarketInvestigator::MARKET)
		CreateThread(0, 0, (thread_open_stalls), (void*)this, 0, 0);
	else
		CreateThread(0, 0, (thread_open_shops), (void*)this, 0, 0);
	get_data_for = NOTHING;
	
}

void MarketInvestigator::on_leave_18(void)
{
	//printf("leave 18\n");
	is_receiving = false;
	vec.push_back(wstring(L"stall_owner"));
	vec.push_back(now_stall_owner);
	store_a_record_to_mongo_db();
	return ;
}

void MarketInvestigator::store_a_record_to_mongo_db(void)
{
	wstring params = L"json={";
	int vec_size = vec.size();
	for(int i=0;i<vec_size;i+=2)
	{
		if(i!=0)
			params += wstring(L",");
		params += L"\"";
		params += vec[i];
		params += wstring(L"\":\"");
		params += vec[i+1];
		params += L"\"";
	}
	params += L"}";

	char url[50000]={0};
	int wsl = wcslen(params.c_str());
	int rl = WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)params.c_str(), wsl, (LPSTR)url, wsl*3, 0, 0);
	
	CURL *curl;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/c.php");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, url);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	vec.clear();
}

void MarketInvestigator::set_send_packet_func(void* pf)
{
	pFuncSendPacket = (void (__cdecl *)(char* , unsigned int))pf;
}

