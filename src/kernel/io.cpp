#include "io.h"
#include "kernel.h"
#include "handles.h"
#include "stdio.h"
#include "file_system.h"



void HandleIO(kiv_os::TRegisters &regs) {

	//V ostre verzi pochopitelne do switche dejte volani funkci a ne primo vykonny kod

	switch (regs.rax.l) {
			case kiv_os::scCreate_File: {
				Create_File(regs);
			}
			break;	//scCreateFile


		case kiv_os::scWrite_File: {
				DWORD written;
				HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
				regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
				if (!regs.flags.carry) regs.flags.carry = !WriteFile(hnd, reinterpret_cast<void*>(regs.rdi.r), (DWORD)regs.rcx.r, &written, NULL);
				if (!regs.flags.carry) regs.rax.r = written;
					else regs.rax.r = GetLastError();

			}
			break; //scWriteFile

		case kiv_os::scRead_File: {
			DWORD read;
			HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
			regs.flags.carry = hnd == INVALID_HANDLE_VALUE;
			if (!regs.flags.carry) regs.flags.carry = !ReadFile(hnd, reinterpret_cast<void*>(regs.rdi.r), (DWORD)regs.rcx.r, &read, NULL);
			if (!regs.flags.carry) regs.rax.r = read;
			else regs.rax.r = GetLastError();

		}
			break; //scReadFile

		case kiv_os::scClose_Handle: {
				HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
				regs.flags.carry = !CloseHandle(hnd);
				if (!regs.flags.carry) Remove_Handle(regs.rdx.x);				
					else regs.rax.r = GetLastError();
			}

			break;	//CloseFile
	}
}

void Create_File(kiv_os::TRegisters &regs) {
	HANDLE result = CreateFileA((char*)regs.rdx.r, GENERIC_READ | GENERIC_WRITE, (DWORD)regs.rcx.r, 0, OPEN_EXISTING, 0, 0);
	//zde je treba podle Rxc doresit shared_read, shared_write, OPEN_EXISING, etc. podle potreby
	std::string name = reinterpret_cast<char *>(regs.rdx.r);
	// TO DO: p�ed�lat na file descriptor
	//HRESULT create_file = createChild(getRoot()->filePath, name, false);
	HRESULT create_file = S_OK;
	regs.flags.carry = result == INVALID_HANDLE_VALUE;

	if (!regs.flags.carry && create_file == S_OK) { 
		regs.rax.x = Convert_Native_Handle(result); 
	}
	else {
		regs.rax.r = GetLastError();
	}
}

