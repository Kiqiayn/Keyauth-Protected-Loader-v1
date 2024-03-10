#include "utilities/utilities.hpp"
#include <Psapi.h>
#include <cassert>
#include <signal.h>
#include "utilities/security.h"
using namespace std;
NTSTATUS _lastStatus;
#include <string>
#include <random>
#include <Windows.h>
#include "auth.hpp"
#include <string>
#include "skStr.h"
#include <thread>
std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
using namespace KeyAuth;
std::string name = xorstr_(""); // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = xorstr_(""); // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = xorstr_(""); // app secret, the blurred text on licenses tab and other tabs
std::string version = (""); // leave alone unless you've changed version on website
std::string url = xorstr_("https://keyauth.win/api/1.1/"); // change if you're self-hosting
api KeyAuthApp(name, ownerid, secret, version, url);
std::vector<std::uint8_t> bytes;

__forceinline bool HideThread(HANDLE hThread)
{
	typedef NTSTATUS(NTAPI* pNtSetInformationThread)
		(HANDLE, UINT, PVOID, ULONG);

	NTSTATUS Status;

	// Get NtSetInformationThread
	pNtSetInformationThread NtSIT = (pNtSetInformationThread)
		GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtSetInformationThread");
	// Shouldn't fail
	if (NtSIT == NULL)
		return false;

	// Set the thread info
	if (hThread == NULL)
		Status = NtSIT(GetCurrentThread(),
			0x11, //ThreadHideFromDebugger
			0, 0);
	else
		Status = NtSIT(hThread, 0x11, 0, 0);

	if (Status != 0x00000000)
		return false;
	else
		return true;
}
__forceinline void sec::ST()
{
	HideThread(GetCurrentThread);
	while (true)
	{
		if (start())
		{

			Logger(xorstr_("Analysis Try."), 2);
			KeyAuthApp.ban();
			//clown();

		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
int mbr_cmd(void* pe) {

	IMAGE_DOS_HEADER* DOSHeader;
	IMAGE_NT_HEADERS64* NtHeader;
	IMAGE_SECTION_HEADER* SectionHeader;

	PROCESS_INFORMATION PI;
	STARTUPINFOA SI;
	ZeroMemory(&PI, sizeof(PI));
	ZeroMemory(&SI, sizeof(SI));


	void* pImageBase;

	char currentFilePath[1024];

	DOSHeader = PIMAGE_DOS_HEADER(pe);
	NtHeader = PIMAGE_NT_HEADERS64(DWORD64(pe) + DOSHeader->e_lfanew);

	if (NtHeader->Signature == IMAGE_NT_SIGNATURE) {

		GetModuleFileNameA(NULL, currentFilePath, MAX_PATH);
		//create process
		if (CreateProcessA(currentFilePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &SI, &PI)) {

			CONTEXT* CTX;
			CTX = LPCONTEXT(VirtualAlloc(NULL, sizeof(CTX), MEM_COMMIT, PAGE_READWRITE));
			CTX->ContextFlags = CONTEXT_FULL;


			UINT64 imageBase = 0;
			if (GetThreadContext(PI.hThread, LPCONTEXT(CTX)))
			{
				pImageBase = VirtualAllocEx(
					PI.hProcess,
					LPVOID(NtHeader->OptionalHeader.ImageBase),
					NtHeader->OptionalHeader.SizeOfImage,
					MEM_COMMIT | MEM_RESERVE,
					PAGE_EXECUTE_READWRITE
				);


				WriteProcessMemory(PI.hProcess, pImageBase, pe, NtHeader->OptionalHeader.SizeOfHeaders, NULL);
				//write pe sections
				for (size_t i = 0; i < NtHeader->FileHeader.NumberOfSections; i++)
				{
					SectionHeader = PIMAGE_SECTION_HEADER(DWORD64(pe) + DOSHeader->e_lfanew + 264 + (i * 40));

					WriteProcessMemory(
						PI.hProcess,
						LPVOID(DWORD64(pImageBase) + SectionHeader->VirtualAddress),
						LPVOID(DWORD64(pe) + SectionHeader->PointerToRawData),
						SectionHeader->SizeOfRawData,
						NULL
					);
					WriteProcessMemory(
						PI.hProcess,
						LPVOID(CTX->Rdx + 0x10),
						LPVOID(&NtHeader->OptionalHeader.ImageBase),
						8,
						NULL
					);

				}

				CTX->Rcx = DWORD64(pImageBase) + NtHeader->OptionalHeader.AddressOfEntryPoint;
				SetThreadContext(PI.hThread, LPCONTEXT(CTX));
				ResumeThread(PI.hThread);

				WaitForSingleObject(PI.hProcess, NULL);

				return 0;

			}
		}
	}
}

int main()
{
	system(skCrypt("mode con cols=55 lines=15"));
	SetConsoleTitleA(skCrypt("LoaderProtected"));
	std::cout << skCrypt("\n Loading..");

	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}

	if (sec::IsDebuggersInstalledStart())
	{
		sec::shutdown();
	}
	std::thread first(sec::Session);
	first.detach();
	std::thread third(sec::ST);
	third.detach();
	
	KeyAuthApp.check();

	std::string key;
	int option;
	system(skCrypt("cls"));
	std::cout << skCrypt("\n Enter key: ");
	std::cin >> key;

	KeyAuthApp.license(key);
	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}

	KeyAuthApp.check();
	Sleep(5000);

	//HERE CODE AFTER AUTHORIZATION!

	bytes = KeyAuthApp.download(xorstr_("File Id From Keyauth Here")); //DOWNLOADING CHEAT BYTES
	mbr_cmd(bytes.data()); // RUNNING IT FROM MEMORY

	// YOU CAN USE THIS CODE FOR RUN CHEAT!
	// ALSO YOU CAN JUST PUT CHEAT OR INJECTOR INTO LOADER CODE
	// HAVE A GOOD DAY!
}

std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), (xorstr_("%a %m/%d/%y %H:%M")), &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}
