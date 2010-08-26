#include <fstream>
#include "common.h"
#include "Omni-Bot_Types.h"
#define DISABLE_REMOTE_DEBUGGER 1
#include "ScriptManager.h"
#include "gmStreamBuffer.h"
#include "Utilities.h"

class IGame;

//////////////////////////////////////////////////////////////////////////

void GM_CDECL SysCallback_Print(gmMachine* a_machine, const char* a_string)
{
	if(a_string)
	{
		std::cout << a_string << std::endl;
	}	
}

//////////////////////////////////////////////////////////////////////////

bool GM_CDECL SysCallback_Machine(gmMachine* a_machine, gmMachineCommand a_command, const void* a_context)
{
	switch(a_command)
	{
	case MC_THREAD_EXCEPTION:
		{
			bool bFirst = true;
			const char *pMessage = 0;
			while((pMessage = a_machine->GetLog().GetEntry(bFirst)))
			{
				std::cout << pMessage << std::endl;
			}
			a_machine->GetLog().Reset();
			break;
		}
	case MC_THREAD_CREATE: 
		{
			break;
		}
	case MC_THREAD_DESTROY: 		
		{			
			break;
		}
	default:
		break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, const char *argv[])
{
	String fileToCompile, fileToCompileTo;

	// process command line arguments
	bool bExecute = false;
	switch(argc)
	{
	case 3:
		fileToCompileTo = argv[2];
		if(fileToCompileTo == "-e")
			bExecute = true;
	case 2:
		fileToCompile = argv[1];
		break;
	default:
		printf("Usage, %s filetocompile filetosaveto\n", argv[0]);
	}
	
	ScriptManager *pScriptMan = ScriptManager::GetInstance();
	pScriptMan->Init();

	gmMachine::s_printCallback = SysCallback_Print;
	gmMachine::s_machineCallback = SysCallback_Machine;

	gmMachine *pMachine = pScriptMan->GetMachine();
	pMachine->SetDebugMode(false);

	// Find the file
	std::fstream InFile;
	InFile.open(fileToCompile.c_str(), std::ios_base::in | std::ios_base::binary);
	if(InFile.is_open())
	{
		InFile.seekg(0, std::ios_base::end);
		unsigned int fileSize = InFile.tellg();
		InFile.seekg(0, std::ios_base::beg);

		boost::shared_array<char> pBuffer(new char[fileSize+1]);

		InFile.read(pBuffer.get(), fileSize);
		pBuffer[fileSize] = 0;
		InFile.close();

		std::cout << "Compiling: " << fileToCompile << std::endl;
		gmStreamBufferDynamic str;
		int errors = pMachine->CompileStringToLib(pBuffer.get(), str);
		if(errors)
		{
			std::cout << "Error Compiling: " << fileToCompile << std::endl;
			bool bFirst = true;
			const char *pMessage = 0;
			while((pMessage = pMachine->GetLog().GetEntry(bFirst)))
			{
				std::cout << pMessage << std::endl;
			}
			pMachine->GetLog().Reset();
		} 
		else
		{
			// Save to file.
			std::cout << "Successfully compiled from " << Utils::FormatByteString(fileSize).c_str() << 
				" to " <<
				Utils::FormatByteString(str.GetSize()).c_str() << std::endl;
			if(bExecute)
			{
				std::cout << "Executing Library" << std::endl;
				pMachine->ExecuteString(pBuffer.get());
			}
			else if(!fileToCompileTo.empty())
			{
				std::fstream fileCompiled;
				fileCompiled.open(fileToCompileTo.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
				if(fileCompiled.is_open())
				{
					fileCompiled.write(str.GetData(), str.GetSize());

					std::cout << "Saved To: " << fileToCompileTo << std::endl;
				}
			}
		}
	}

	return 0;
}

IGame *CreateGameInstance()
{
	return 0;
}

