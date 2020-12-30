#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <sddl.h>
#include <wchar.h>
#include <tchar.h>
#include <locale.h>
#include <string.h>

using namespace std;


const char *sidToTextTok(PSID psid)
{
	// S-rev- + SIA + subauthlen*maxsubauth + terminator
	static char buf[15 + 12 + 12 * SID_MAX_SUB_AUTHORITIES + 1];
	char *p = &buf[0];
	PSID_IDENTIFIER_AUTHORITY psia;
	DWORD numSubAuths, i;

	// Validate the binary SID.

	if (!IsValidSid(psid))
		return FALSE;

	psia = GetSidIdentifierAuthority(psid);

	p = buf;
	p += _snprintf_s(p, 15 + 12 + 12 * SID_MAX_SUB_AUTHORITIES + 1, &buf[sizeof buf] - p, "S-%lu-", 0x0f & *((byte *)psid));

	if ((psia->Value[0] != 0) || (psia->Value[1] != 0))
		p += _snprintf_s(p, 15 + 12 + 12 * SID_MAX_SUB_AUTHORITIES + 1, &buf[sizeof buf] - p, "0xhxhxhxhxhxhx",
		(USHORT)psia->Value[0], (USHORT)psia->Value[1],
			(USHORT)psia->Value[2], (USHORT)psia->Value[3],
			(USHORT)psia->Value[4], (USHORT)psia->Value[5]);
	else
		p += _snprintf_s(p, 15 + 12 + 12 * SID_MAX_SUB_AUTHORITIES + 1, &buf[sizeof buf] - p, "%lu", (ULONG)(psia->Value[5]) +
		(ULONG)(psia->Value[4] << 8) + (ULONG)(psia->Value[3] << 16) +
			(ULONG)(psia->Value[2] << 24));

	// Add SID subauthorities to the string.

	numSubAuths = *GetSidSubAuthorityCount(psid);
	for (i = 0; i < numSubAuths; ++i)
		p += _snprintf_s(p, 15 + 12 + 12 * SID_MAX_SUB_AUTHORITIES + 1, &buf[sizeof buf] - p, "-%lu", *GetSidSubAuthority(psid, i));

	return buf;
}

void PrintPermissionsTok(PACL DACL)
{
	DWORD dwCount = 0;
	ACCESS_ALLOWED_ACE		*ACE;

	
	if (IsValidAcl(DACL) == TRUE) {
		fprintf(stdout, "[i]    |\n");
		fprintf(stdout, "[i]    +-+-> Default DACL for new objects created by this user\n");

		// Now for each ACE in the DACL
		for (dwCount = 0; dwCount < DACL->AceCount; dwCount++) {
		
			if (GetAce(DACL, dwCount, (LPVOID*)&ACE)) {
					
				SID *sSID = (SID*)&(ACE->SidStart);
				if (sSID != NULL)
				{
					DWORD dwSize = 2048;
					char lpName[2048];
					char lpDomain[2048];
					SID_NAME_USE SNU;

					switch (ACE->Header.AceType) {
						// Allowed ACE
					case ACCESS_ALLOWED_ACE_TYPE:
						// Lookup the account name and print it.										
						
						if (!LookupAccountSidA(NULL, sSID, lpName, &dwSize, lpDomain, &dwSize, &SNU)) {

							DWORD dwResult = GetLastError();
							if (dwResult == ERROR_NONE_MAPPED) {
								fprintf(stdout, "[%d]    |\n", dwCount);
								fprintf(stdout, "[%d]    +---+-> Allowed - NONMAPPED - SID %s\n", dwCount, sidToTextTok(sSID));
							}
							else if (dwResult != ERROR_NONE_MAPPED) {
								fprintf(stderr, "[!] LookupAccountSid Error 	%u\n", GetLastError());
								fprintf(stdout, "[%d]    |\n", dwCount);
								fprintf(stdout, "[%d]    +---+-> Allowed - ERROR     - SID %s\n", dwCount, sidToTextTok(sSID));
								//return;
							}
							else {
								continue;
							}
						}
						else {

							fprintf(stdout, "[%d]    |\n", dwCount);
							fprintf(stdout, "[%d]    +---+-> Allowed - %s\\%s\n", dwCount, lpDomain, lpName);

							char strUserFromPID[1024];
							memset(strUserFromPID, 0x00, 1024);
							if (!strcmp(lpDomain, "BUILTIN") == 0 && !strcmp(lpName, "OWNER RIGHTS") == 0 && !strcmp(lpDomain, "NT AUTHORITY") == 0 && !strcmp(lpDomain, "NT SERVICE") == 0) {
								fprintf(stdout, "[%d]      |\n", dwCount);
								fprintf(stdout, "[%d]      +-+-+-> Alert!\n", dwCount);
							}
						}

						// print out the ACE mask
						fprintf(stdout, "[%d]        |\n", dwCount);
						fprintf(stdout, "[%d]        +-> Permissions - ", dwCount);

						if (ACE->Mask & GENERIC_ALL) fprintf(stdout, "| All");
						if (ACE->Mask & GENERIC_EXECUTE) fprintf(stdout, "| Execute");
						if (ACE->Mask & GENERIC_READ) fprintf(stdout, "| Read");
						if (ACE->Mask & GENERIC_WRITE) fprintf(stdout, "| Write");
						if (ACE->Mask & DELETE) fprintf(stdout, "| Delete");
						if (ACE->Mask & READ_CONTROL) fprintf(stdout, "| Read control");
						if (ACE->Mask & SYNCHRONIZE) fprintf(stdout, "| Sync");
						if (ACE->Mask & WRITE_DAC) fprintf(stdout, "| Modify DACL");
						if (ACE->Mask & WRITE_OWNER) fprintf(stdout, "| Write Owner");
						if (ACE->Mask & STANDARD_RIGHTS_ALL) fprintf(stdout, "| All");
						if (ACE->Mask & STANDARD_RIGHTS_EXECUTE) fprintf(stdout, "| Execute");
						if (ACE->Mask & STANDARD_RIGHTS_READ) fprintf(stdout, "| Read Control");
						if (ACE->Mask & STANDARD_RIGHTS_REQUIRED)  fprintf(stdout, "| Write");
						if (ACE->Mask & STANDARD_RIGHTS_WRITE) fprintf(stdout, "| Read Control");


						fprintf(stdout, "\n");
						break;

						// Denied ACE
					case ACCESS_DENIED_ACE_TYPE:
						break;

						// Uh oh
					default:
						break;

					}
				}
			}
			else {
				DWORD dwError = GetLastError();
				fprintf(stderr, "[!] Error - %d - GetAce\n", dwError);
				return;
			}

		}
	}
	else {
		DWORD dwError = GetLastError();
		fprintf(stderr, "[!] Error - %d - IsValidAcl\n", dwError);
		return;
	}

}

int getDacleString(HKEY root, LPCWSTR subkey)
{
	SECURITY_INFORMATION lable = DACL_SECURITY_INFORMATION;
	HKEY hkey;
	DWORD psdsize = 1;
	REGSAM sam = KEY_READ | KEY_ENUMERATE_SUB_KEYS;

	LONG err = RegOpenKeyEx(root, subkey, 0, sam, &hkey);
	if (err != ERROR_SUCCESS)
	{
		if (err == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return TRUE;
		}
		else {
			printf("Error opening key.\n");
			return FALSE;
		}
	}

	if (err == 0 && hkey != 0) {
		PSID group;
		BOOL groupDefaulted = 0;
		LPSTR groupSidString;

		RegGetKeySecurity(hkey, lable, NULL, &psdsize);
		PSECURITY_DESCRIPTOR psd = LocalAlloc(LMEM_FIXED, psdsize);
		RegGetKeySecurity(hkey, lable, psd, &psdsize);
		LPBOOL lpbDaclPresent;
		PACL pAcl;
		
		LPVOID pAclInformation;
		DWORD cbAclInfo;
		ACL_INFORMATION_CLASS dwAclInfoClass;
		
		PSID pOwnerSID = NULL;
		PSID pGroupSID = NULL;
		PACL pDACL = NULL;
		PACL pSACL = NULL;
		BOOL bOwnerDefaulted = FALSE;
		BOOL bGroupDefaulted = FALSE;
		BOOL bDaclPresent = FALSE;
		BOOL bDaclDefaulted = FALSE;
		BOOL bSaclPresent = FALSE;
		BOOL bSaclDefaulted = FALSE;

		if (psd) {
			GetSecurityDescriptorOwner(psd, &pOwnerSID, &bOwnerDefaulted);
			GetSecurityDescriptorGroup(psd, &pGroupSID, &bGroupDefaulted);
			GetSecurityDescriptorDacl(psd, &bDaclPresent, &pDACL, &bDaclDefaulted);
			GetSecurityDescriptorSacl(psd, &bSaclPresent, &pSACL, &bSaclDefaulted);
		}
		PrintPermissionsTok(pDACL);
	}
	else return NULL;
}



static HKEY Translate(char* key)
{
	if (strcmp(key, "HKEY_LOCAL_MACHINE"))
		return HKEY_LOCAL_MACHINE;
	else if (strcmp(key, "HKEY_CLASSES_ROOT"))
		return HKEY_CLASSES_ROOT;
	else if (strcmp(key, "HKEY_USERS"))
		return HKEY_USERS;
	else if (strcmp(key, "HKEY_CURRENT_CONFIG"))
		return HKEY_CURRENT_CONFIG;
	else if (strcmp(key, "HKEY_CURRENT_USER"))
		return HKEY_CURRENT_USER;
	return NULL;
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Rus");
	
	if (argc == 3)
	{
		printf("\n1\n");
		getDacleString(Translate(argv[1]), convertCharArrayToLPCWSTR(argv[2]));
	}
	else if (argc == 2)
	{
		printf("\n2\n");
		getDacleString(Translate(argv[1]), NULL);
	}
	else
	{
		printf("\nNot arguments or too many arguments\n");
	}
}

