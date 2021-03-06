#include "stdafx.h"
#include "parallelport.h"


namespace WinParallelPort {

	PORT_INFO_1 ports[3];
	USHORT portAddress = 0xcff8; //intialize with standard old-school base addresss

	bool sendTrig(int trigger) {
		DlPortWritePortUchar(portAddress, (USHORT)trigger);
		Sleep(5);
		DlPortWritePortUchar(portAddress, 0);
		return true;
	}
	
	std::vector<PORT_INFO_2> enumPorts() {
		
		DWORD bufferSize;
		DWORD nbrOfPorts;
		PORT_INFO_2* portInfos;
		//find out how large the buffer that stores the PORT_INFO objects should be
		EnumPorts(NULL, 2, NULL, 0, &bufferSize, &nbrOfPorts);
		portInfos = (PORT_INFO_2*)malloc(bufferSize); // then allocate enough memory for it
		BOOL success = EnumPorts(NULL, 2, (LPBYTE)portInfos, bufferSize, &bufferSize, &nbrOfPorts); //portInfos is now a pointer to contiguous PORT_INFO_2 structs

		//transfer the structs to the more friendly vector
		std::vector<PORT_INFO_2> ports; 
		for (int i = 0; i < nbrOfPorts; i++) {
			if (portInfos[i].pPortName != nullptr) {
				ports.push_back(portInfos[i]);
			}
		}
		return ports;
	}

	void setupAddress(std::string address) {
		portAddress = (USHORT)std::stoi(address, nullptr, 16);
	}

	bool process(rapidjson::Document& message) {
		if (!message.HasMember("action") || !message["action"].IsString() || !message.HasMember("payload")) {
			return false;
		}
		std::string action = message["action"].GetString();

		if (action == "setup") {
			if (message["payload"].IsString()) {
				try
				{
					setupAddress(message["payload"].GetString());
				}
				catch (const std::exception& ex)
				{
					return false;
				}
			}
			else {
				return false;
			}
		}
		else if (action == "trigger") {
			if (message["payload"].IsInt()) {
				sendTrig(message["payload"].GetInt());
			}
			else return false;
		}
		
	}
}