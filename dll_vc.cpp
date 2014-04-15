// dll_vc.cpp : Defines the entry point for the DLL application.
//
#include <stdio.h>
#include "stdafx.h"
#include "market_investigator.h"

char* ori[50];
void* head_addr;
int flag=0;
char* str1;
char* str2;
unsigned int the_uint1;
unsigned int the_uint2;
wchar_t* wstr;
MarketInvestigator *pMarketInvestigator;

unsigned int socket_id;

void hook_call()
{
	
	//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),str1,wcslen((unsigned short*)str1),&ws,NULL);
	printf("%s\n", str1);
}

void print_chat()
{
	DWORD ws;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),str2,wcslen((unsigned short*)str2),&ws,NULL);//to print unicode
}

void asm_head()
{
LL:
	__asm{
		mov [head_addr],offset LL
	}
	if(flag == 1)
		return;
	__asm{
		push eax
		mov eax,[esp+4]
		mov str1,eax
		pop eax
	}
	__asm{
		pushad
		pushfd
	}
	
	hook_call();
	__asm{
		popfd
		popad
	}
	__asm{
		add esp, 0x10
		lea edx, dword ptr ss:[esp+0x8]
		jmp		[ori]
	}
}

void asm_head_print_chat()
{
LL:
	__asm{
		mov [head_addr],offset LL
	}
	if(flag == 1)
		return;
	__asm{
		push eax
		mov str2,edx
		pop eax
	}
	__asm{
		pushad
		pushfd
	}
	
	print_chat();
	__asm{
		popfd
		popad
	}
	__asm{
		add esp, 0x10
		lea ecx, dword ptr ss:[esp+0x54]
		jmp		[ori+4]
	}
}

void asm_head_on_ext_splite_wstring()
{
LL2:
	__asm{
		mov [head_addr],offset LL2
	}
	if(flag == 1)
		return;
	__asm{
		push eax
		mov wstr,eax
		pop eax
	}
	__asm{
		pushad
		pushfd
	}
	pMarketInvestigator->on_receive_market_list(wstr);
	__asm{
		popfd
		popad
	}
	__asm{
		mov edi, eax
		mov eax, dword ptr ds:[esi]
		mov edx,dword ptr ds:[eax+0x60]
		jmp [ori+8]
	}
}

void asm_head_on_receive_18()
{
LL3:
	__asm{
		mov [head_addr],offset LL3
	}
	if(flag == 1)
		return;
	__asm{
		pushad
		pushfd
	}
	pMarketInvestigator->on_receive_18();
	__asm{
		popfd
		popad
	}
	__asm{
		cmp dword ptr ss:[esp+0x19C],0x7
		jmp [ori+0xC]
	}
}

void asm_head_on_leave_18()
{
LL4:
	__asm{
		mov [head_addr],offset LL4
	}
	if(flag == 1)
		return;
	__asm{
		pushad
		pushfd
	}
	pMarketInvestigator->on_leave_18();
	__asm{
		popfd
		popad
	}
	__asm{
		mov ecx, dword ptr ss:[esp+0x174]
		jmp [ori+0x10]
	}
}

void asm_head_on_receive_key()
{
LL5:
	__asm{
		mov [head_addr],offset LL5
	}
	if(flag == 1)
		return;
	__asm{
		mov str1,edx
		pushad
		pushfd
	}
	pMarketInvestigator->on_receive_key(str1);
	__asm{
		popfd
		popad
	}
	__asm{
		xor edi,edi
		mov dword ptr ss:[ebp-0x34],edx
		jmp [ori+0x14]
	}
}

void asm_head_on_receive_value()
{
LL6:
	__asm{
		mov [head_addr],offset LL6
	}
	if(flag == 1)
		return;
	__asm{
		push ebx
		push eax
		mov ebx,[ecx]
		mov str1,ebx
		lea eax,dword ptr ds:[edx-0x1]
		mov the_uint1,eax
		mov ebx, [ecx+0x8]
		mov the_uint2,ebx
		pop eax
		pop ebx
		pushad
		pushfd
	}
	pMarketInvestigator->on_receive_value(the_uint1, str1, the_uint2);
	__asm{
		popfd
		popad
	}
	__asm{
		lea eax,dword ptr ds:[edx-0x1]
		cmp eax,0x8
		jmp [ori+0x18]
	}
}

void acp2utf8(char* input, char* output,int size)
{
	char unistring[4096]={0};
	int result = MultiByteToWideChar(CP_ACP, 0, (char*)(input), -1, (unsigned short *)unistring, (size_t)(sizeof(unistring)));
    WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)(unistring), -1, (LPSTR)output, size, 0, 0);
}



void send_packet(char* packet, unsigned int length)
{
	void (__stdcall *pFuncWs2Send)(unsigned int,char*,unsigned int,unsigned int);
	unsigned int* (*pFuncGetDynamicStructure)(void);
	void (*pFuncHashEncrypt)(char*,char*,unsigned int);
	pFuncHashEncrypt = (void (__cdecl *)(char*,char*,unsigned int))0x32169830;
	

	pFuncGetDynamicStructure = (unsigned int* (__cdecl *)(void))0x110430AE;
	unsigned int* tmpp = pFuncGetDynamicStructure();
	// update ws2_32.send base address
	__asm{
		push eax
		push ebx
		mov eax,tmpp
		mov eax, dword ptr [eax+0x28]
		mov ebx, dword ptr [eax+0x38]

		mov socket_id,ebx
		mov eax, dword ptr [eax+0x34]
		mov eax, dword ptr [eax+0x8]
		mov tmpp,eax
		
		xor ebx,ebx
		mov eax, dword ptr [ebx+0x1062C130]
		mov pFuncWs2Send,eax
		pop ebx
		pop eax
	}
	pFuncHashEncrypt = (void (__cdecl *)(char*,char*,unsigned int))tmpp;
	pFuncHashEncrypt((char*)0xA3924B8,packet,length-2);
	pFuncWs2Send(socket_id,packet,length,0x0);
}

unsigned int got_socket_response(char* resp, unsigned int len)
{
	unsigned int (__stdcall *pFuncWs2Recv)(unsigned int,char*,unsigned int,unsigned int);
	unsigned int* tmpp;
	void (__cdecl *pFuncRecv)(void);
	__asm{
		push eax
		xor eax,eax
		mov eax, dword ptr [eax+0x1062B148]
		mov tmpp,eax
		pop eax
	}
	pFuncWs2Recv = (unsigned int (__stdcall *)(unsigned int,char*,unsigned int,unsigned int))tmpp;
	
	pFuncRecv = (void (__cdecl *)(void))0x10618E30;
	pFuncRecv();
	return 1;
}

void send_market_query()
{
	char packet[1024]={0};
	//stuff data
	packet[0] = 0x0A; //packet head
	packet[0x15] = 0x04; //unknow
	packet[0x17] = 0x02; //type: 4 byte data
	*((unsigned int*)(packet+0x18)) = 0x3E0;
	packet[0x1C] = 0x02;
	*((unsigned int*)(packet+0x1D)) = 0x017;
	packet[0x21] = 0x05;
	*((unsigned int*)(packet+0x26)) = 0x4092D800;
	packet[0x2A] = 0x05;
	packet[0x33] = 0xEE;
	packet[0x34] = 0xEE;
	send_packet(packet,0x35);
	char resp[4096] = {0};
	//printf("len: %d\n", got_socket_response(resp,0x1000));
	//printf("%s\n",resp);
}

/**
* Last update inject info: 2014/04/14
*/
void auto_chat()
{
	char packet[1024]={0};
	char str[] = "CÖÐBÓ¢A~!#$%FUK";
	char utf8str[1024] = {0};
	char encoded[1024]={0};
	char encoded_unicode[2048]={0};
	
	void (*pFunc)(char*,char*) ;
	pFunc = (void (__cdecl *)(char*,char *))0x1104443B;

	acp2utf8(str,utf8str,1024);
	(*pFunc)(encoded,utf8str);

	unsigned int len = MultiByteToWideChar(CP_ACP, 0, (char*)(encoded+1), -1, (unsigned short *)encoded_unicode, (size_t)(sizeof(encoded_unicode)));

	unsigned int packet_len = 0x26+len*2+5;
	//stuff data
	packet[0] = 0x0A; //packet head
	packet[0x15] = 0x04; //unknow
	packet[0x17] = 0x02; //type: 4 byte data
	packet[0x18] = 0x01; //unknow
	packet[0x1C] = 0x02; //type: 4 byte data
	packet[0x1D] = 0x01; //1: near; 2 all map; 8 school
	packet[0x21] = 0x07; //type: string?v
	*(unsigned int*)(packet+0x22) = len*2;
	memcpy(packet+0x26,encoded_unicode,len*2);
	packet[0x26+len*2] = 0x02;
	*(unsigned int*)(packet+0x26+len*2+1) = 0;
	packet[0x26+len*2+5]=0xEE;
	packet[0x26+len*2+6]=0xEE;
	send_packet(packet,0x26+len*2+6+1);
}

void inject_code(char* pos, char* byte_code, unsigned int byte_code_len, void (*pHead)(void))
{
    // inject address
	int i;
	int jmp_op_addr;
	char* start = pos-10;
	int seg_size = 0x100;
	
	unsigned long output;
	flag = 1;
	pHead(); // get pos first
	flag = 0;

	jmp_op_addr = (int)head_addr - (int)(pos+5);
	memcpy(byte_code+1, &jmp_op_addr, 4);
	printf("strlen byte code: %d\n", byte_code_len);
	for(i=0;i<0x50;i++)
        printf(" %2x", *(pos+i));
	printf("\n\n\n");
	for(i=0;i<byte_code_len;i++)
        printf(" %2x", (0x0FF&(*(byte_code+i))));
	//ori = (char*)(pos+byte_code_len);
	//printf("inject_code at %x, and return %x\n", pos, ori);
	VirtualProtect(start, seg_size, PAGE_EXECUTE_READWRITE, &output); // inject code
	memcpy(pos, byte_code, byte_code_len);
	VirtualProtect(start, seg_size, PAGE_EXECUTE_READ, &output); // inject code
}

void cmd_thread()
{
	printf("cmd thread has been created!\n");
	char cmd[1024]={0};
	while(scanf("%s",cmd))
	{
		printf("%s\n",cmd);
		if(strcmp(cmd,"chat") == 0)
			auto_chat();
		else if(strcmp(cmd,"market") == 0)
			pMarketInvestigator->send_query_all_market();
		else if(strcmp(cmd,"next") == 0)
			pMarketInvestigator->next_signal = true;
	}
}

void create_cmd_thread()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cmd_thread, 0, 0, 0);
}

/**
* Last update inject info: NOD, NOD, 
* When need to inject code, fill follow field: target_addr,
*							asm_patch_head(call your function inside),
							cover_next_instruction_bytes(next 5 bytes for opcode of call instruction, n for ruined instructions filled by nops)
*/
void inject()
{
	unsigned int target_addr[] = {
		0x1137F851,// on_%x
		0x1138248B,// char details
		0x118AFDD1,// ext_splite_wstring, updated at 2014/04/14
		0x1060A11A,// on_receving_18, updated at 2014/04/15
		0x1060A2E8,// on_leave_18, updated at 2014/04/15
		0x10607482,// on_receive_key, updated at 2014/04/15
		0x106074B4 // on_receive_value, updated at 2014/04/15
	};

	void (*pHead_func[])(void) = {
		asm_head,
		asm_head_print_chat,
		asm_head_on_ext_splite_wstring,
		asm_head_on_receive_18,
		asm_head_on_leave_18,
		asm_head_on_receive_key,
		asm_head_on_receive_value
	};

	int cover_next_instruction_bytes[] = {7, 7, 7, 8, 7, 5, 6};


	char byte_code[] = {0xE9, 0x6C, 0xC4, 0xFF, 0x0E, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	for(int i=2;i<7;i++)
	{
		ori[i] = (char*)((target_addr[i])+cover_next_instruction_bytes[i]); // jump back addr
		inject_code((char*)(target_addr[i]), byte_code, cover_next_instruction_bytes[i], pHead_func[i]);
	}

	printf("finish inject\n");
	int a;
}

void show_console()
{
	AllocConsole();
    freopen("CONOUT$","w+t",stdout);
    freopen("CONIN$","r+t",stdin);
}

void close_console()
{
	fclose(stdout);
    fclose(stdin);
    FreeConsole();
}



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
			show_console();
			pMarketInvestigator = new MarketInvestigator();
			pMarketInvestigator->set_send_packet_func(send_packet);
			inject();
			create_cmd_thread();
			printf("DLL_PROCESS_ATTACH\n");
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            printf("DLL_PROCESS_DETACH\n");
			close_console();
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            printf("DLL_THREAD_ATTACH\n");
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
			printf("DLL_THREAD_DETACH\n");
            break;
    }
    return TRUE; // succesful
}

