// SGDAN.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include<TlHelp32.h>
#include <tchar.h>

using namespace std;

int GameStatus;
int ResultofMatch;
unsigned int RNG0;
int SGLobbyName;
int SGLobbySize;
int Player1or2;
__int64 OwnSteamID;
__int64 OpponentSteamID;

int SGDANState = 99;
int SGDANStateOLD;

void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 4 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR)' ',
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}




DWORD GetModuleBaseAddress(DWORD pID) {
	DWORD dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
	MODULEENTRY32 ModuleEntry32 = { 0 };
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
	{
		do {
			if ((_tcscmp(ModuleEntry32.szModule, "Skullgirls.exe") == 0) or (_tcscmp(ModuleEntry32.szModule, "SkullGirls.exe") == 0)) // if Found Module matches Module we look for -> done!
			{
				dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

void ReadMemory()
{	
	HWND hwnd = FindWindowA(NULL, "Skullgirls Encore");
	if (hwnd == NULL)
	{	
		SGDANState = 99;
	}
	else {
		DWORD ProcID;
		GetWindowThreadProcessId(hwnd, &ProcID);
		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcID);
		if (ProcID == NULL)
		{
			SGDANState = 99;
		}
		else{
			SGDANState = 0;
			DWORD gameBaseAddress = GetModuleBaseAddress(ProcID);
			DWORD BaseAdressGame;
			DWORD BaseAdressLobby;
			ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + 0x00852178), &BaseAdressGame, sizeof(BaseAdressGame), 0);
			ReadProcessMemory(processHandle, (LPVOID)(BaseAdressGame + 0x168 ), &GameStatus, sizeof(GameStatus), 0);
			ReadProcessMemory(processHandle, (LPVOID)(BaseAdressGame + 0x3E4), &ResultofMatch, sizeof(ResultofMatch), 0);
			ReadProcessMemory(processHandle, (LPVOID)(BaseAdressGame + 0x400), &RNG0, sizeof(RNG0), 0);
			ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + 0x0043C758), &BaseAdressLobby, sizeof(BaseAdressLobby), 0);
			ReadProcessMemory(processHandle, (LPVOID)(BaseAdressLobby + 0x14), &BaseAdressLobby, sizeof(BaseAdressLobby), 0);
			ReadProcessMemory(processHandle, (LPVOID)(BaseAdressLobby + 0x7EC), &SGLobbyName, sizeof(SGLobbyName), 0);
			ReadProcessMemory(processHandle, (LPVOID)(BaseAdressLobby + 0x7FC), &SGLobbySize, sizeof(SGLobbySize), 0);
			ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + 0x000339E8), &Player1or2, sizeof(Player1or2), 0);
			ReadProcessMemory(processHandle, (LPVOID)(Player1or2), &Player1or2, sizeof(Player1or2), 0);
			ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + 0x008530B8), &OwnSteamID, sizeof(OwnSteamID), 0);
			ReadProcessMemory(processHandle, (LPVOID)(OwnSteamID + 0x04), &OwnSteamID, sizeof(OwnSteamID), 0);
			ReadProcessMemory(processHandle, (LPVOID)(OwnSteamID + 0x18), &OwnSteamID, sizeof(OwnSteamID), 0);
			ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + 0x001D93E8), &OpponentSteamID, sizeof(OpponentSteamID), 0);
			ReadProcessMemory(processHandle, (LPVOID)(OpponentSteamID), &OpponentSteamID, sizeof(OpponentSteamID), 0);
			SGDANState = GameStatus + RNG0 + SGLobbyName + SGLobbySize;
		}
	}
}

void TextOutput() {
		if (SGDANState == 99) {
			cout << "Can't find a Window!\n\n";
			cout << "Comment: If you run the game and the message still appears, please contact me (ImpDi, I guess?)\n";
		}
		else {
			int NOTsendreason;
			cout << "GameStatus: " << GameStatus << " ";
				if (GameStatus == 4) {
					cout << "(We are playing!)" << endl;
					NOTsendreason = 4;
				} 
				else if (GameStatus == 6) {
					cout << "(We finish the match!)" << endl;
				}
				else {
					cout << "(What are you doing? :> )" << endl;
					NOTsendreason = 3;
				}
			cout << "ResultofMatch: " << ResultofMatch << " ";
				switch (ResultofMatch) {
				case 0:
					cout << "(We are still playing)" << endl;
				break;
				case 1:
					cout << "(Player 1 WINS! Using a timeout, but we don't judge...)" << endl;
				break;
				case 2:
					cout << "(Player 2 WINS! Using a timeout, but we don't judge...)" << endl;
				break;
				case 3:
					cout << "(A DRAW! Using a timeout? Did you play at all?)" << endl;
				break;
				case 4:
					cout << "(Player 1 WINS!)" << endl;
				break;
				case 5:
					cout << "(Player 2 WINS!)" << endl;
				break;
				case 6:
					cout << "(A DRAW!)" << endl;
				break;
				default:
					cout << endl;
				break;
			}
			cout << "RNG0: " << RNG0 << endl;
			cout << "OwnSteamID: " << OwnSteamID << endl;
			cout << "OpponentSteamID: " << OpponentSteamID;
				if (OpponentSteamID == 0) {
					cout << " (We are playing alone?)" << endl;
					NOTsendreason = 5;
				}
				else {
					cout << " (We are playing online!)" << endl;
				}
			cout << "SGLobbyName: " << SGLobbyName << " ";
				if (SGLobbyName == 36) {
					cout << "(First to 5 Wins. You're using the right lobby name!)" << endl;
				}
				else {
					cout << "(You're using the wrong lobby name! Select \"First to 5 Wins.\"!)" << endl;
					NOTsendreason = 1;
				}
			cout << "SGLobbySize: " << SGLobbySize << " ";
				if (SGLobbySize == 2) {
					cout << "(One-to-one! You're using the right lobby size!)" << endl;
				}
				else {
					cout << "(You're using the wrong lobby size! Select 2, play One-to-one!)" << endl;
					NOTsendreason = 2;
				}
			cout << "Player1or2: " << Player1or2 << " ";
				if (Player1or2 == 1) {
					cout << "(We are Player1!)" << endl;
				}
				else{
					cout << "(We are Player2! Or don't even playing...)" << endl;
				}


			cout << endl << "Comment: ";
				if ((OpponentSteamID != 0) and (SGLobbyName == 36) and (SGLobbySize == 2) and (GameStatus == 6)) {
					std::cout << "We are ready to send data! (Someday, Someday...)";
					}
				else {
					cout << "We don't ready send data. Reason: ";
						switch (NOTsendreason) {
						case 1:
							std::cout << "You're playing wrong lobby name! :O";
						break;
						case 2:
							std::cout << "You're playing wrong lobby size! :O";
						break;
						case 3:
							std::cout << "You're don't playing at all! :O";
						break;
						case 4:
							std::cout << "You're still playing! :O";
						break;
						case 5:
							std::cout << "You're still playing alone! :O";
						break;
						}
				}
		}

}



int main() {
	HANDLE CursorHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	cout << "=== Skullgirls Danisen v0.1 (TEST) ===\n";
	//cout << "Waiting for Skullgirls window...\n"; //One day i remember what i gonna write here
	cout << "Press Ctrl+C to exit\n\n";
	while (TRUE){
		ReadMemory();
		if (SGDANState != SGDANStateOLD) {
			SGDANStateOLD = SGDANState;
			ClearScreen(); 
			TextOutput();
		}
		}	
}

