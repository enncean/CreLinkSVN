#include <filesystem>
#include <Windows.h>

#include "CreLinkCore.h"
#include "RevisionSelector.h"

enum class ApplyArgsResult
{
	SUCCESS,
	PATH_NOT_SPECIFIED,
	FILE_NOT_FOUND,
	FILE_ACCESS_ERROR,
	INVALID_ARGUMENT
};

ApplyArgsResult ApplyArgs(CreLinkCore& coreObj, bool& quiet, bool& skipCheck, std::string& resultInfo);
bool SetClipboardText(const std::string& text);

constexpr TCHAR APP_TITLE[] = TEXT("CreLinkSVN");

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CreLinkCore coreObj;
	bool quiet;
	bool skipCheck;
	// Parse arguments.
	{
		std::string resultInfo;
		const ApplyArgsResult result = ApplyArgs(coreObj, quiet, skipCheck, resultInfo);
		if (result != ApplyArgsResult::SUCCESS)
		{
			switch (result)
			{
			case ApplyArgsResult::FILE_NOT_FOUND:
				MessageBox(nullptr,
					TEXT("File or directory not found."),
					APP_TITLE, MB_OK | MB_ICONERROR);
				break;
			case ApplyArgsResult::PATH_NOT_SPECIFIED:
				MessageBox(nullptr,
					TEXT("Need to specify path of file or folder as the first argument."),
					APP_TITLE, MB_OK | MB_ICONERROR);
				break;
			case ApplyArgsResult::FILE_ACCESS_ERROR:
				MessageBox(nullptr,
					TEXT("Failed to access file or directory."),
					APP_TITLE, MB_OK | MB_ICONERROR);
				break;
			case ApplyArgsResult::INVALID_ARGUMENT:
				{
					std::string msg = "Invalid argument '" + resultInfo + "'.";
					MessageBox(nullptr,
						std::wstring(msg.begin(), msg.end()).c_str(),
						APP_TITLE, MB_OK | MB_ICONERROR);
				}
				break;
			}
			return -1;
		}
	}

	if (!skipCheck && !CreLinkCore::CheckSubversionValid())
	{
		MessageBox(nullptr,
			TEXT("\"svn\" command is not available."),
			APP_TITLE, MB_OK | MB_ICONERROR);
		return -1;
	}

	// Read repository and check file.
	{
		const CreLinkCore::ReadRepositoryResult result = coreObj.ReadRepository(skipCheck);
		if (result != CreLinkCore::ReadRepositoryResult::SUCCESS)
		{
			switch (result)
			{
			case CreLinkCore::ReadRepositoryResult::NOT_REPOSITORY:
			case CreLinkCore::ReadRepositoryResult::FILE_CHECK_ERROR:
				MessageBox(nullptr,
					TEXT("Failed to read repository information."),
					APP_TITLE, MB_OK | MB_ICONERROR);
				break;
			case CreLinkCore::ReadRepositoryResult::FILE_NOT_VERSIONED:
				MessageBox(nullptr,
					TEXT("File is not under version control."),
					APP_TITLE, MB_OK | MB_ICONERROR);
				break;
			case CreLinkCore::ReadRepositoryResult::LOG_NOT_EXIST:
				MessageBox(nullptr,
					TEXT("The log can't be read or empty."),
					APP_TITLE, MB_OK | MB_ICONERROR);
				break;
			}
			return -1;
		}
	}

	const INT_PTR selector_ret = SelectorWindow(hInstance, *coreObj.GetCommitLog());
	if (selector_ret == IDOK)
	{
		std::string out = coreObj.GenerateURLWithRev(GetSelectedRevision());
		const bool result = SetClipboardText(out);

		if (result && !quiet)
		{
			std::string result_msg("Path \"" + out + "\" was copied to clipboard.");
			MessageBox(nullptr, std::wstring(result_msg.begin(), result_msg.end()).c_str(), APP_TITLE, MB_OK);
		}
	}
	else if (!quiet)
	{
		MessageBox(nullptr, TEXT("Revision selector canceled."), TEXT(""), MB_OK);
	}
	
	return 0;
}

ApplyArgsResult ApplyArgs(CreLinkCore& coreObj, bool& quiet, bool& skipCheck, std::string& resultInfo)
{
	ApplyArgsResult ret = ApplyArgsResult::SUCCESS;
	std::string path;
	
	quiet = false;
	skipCheck = false;
	resultInfo = "";

	for (int i = 0; i < __argc; i++)
	{
		const std::string a(__argv[i]);
		if (a[0] == '-')
		{
			switch (a[1])
			{
			case 'q':
			case 'Q':
				quiet = true;
				break;

			case 's':
			case 'S':
				skipCheck = true;
				break;
				
			default:
				ret = ApplyArgsResult::INVALID_ARGUMENT;
				resultInfo = a[1];
				break;
			}
		}
		else
		{
			path = a;
		}
	}
	
	if (!path.empty())
	{
		try
		{
			if (std::filesystem::exists(path))
			{
				coreObj.SetPath(path);
			}
			else
			{
				ret = ApplyArgsResult::FILE_NOT_FOUND;
			}
		}
		catch (...)
		{
			ret = ApplyArgsResult::FILE_ACCESS_ERROR;
		}
	}
	else
	{
		ret = ApplyArgsResult::PATH_NOT_SPECIFIED;
	}
	
	return ret;
}

bool SetClipboardText(const std::string& text)
{
	bool result = false;

	const HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (hGlobal != nullptr)
	{
		char* buffer = static_cast<char*>(GlobalLock(hGlobal));
		if (buffer != nullptr)
		{
			strcpy_s(buffer, text.size() + 1, text.c_str());
			GlobalUnlock(hGlobal);

			if (OpenClipboard(nullptr))
			{
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hGlobal);
				CloseClipboard();
				result = true;
			}
		}
	}
	
	return result;
}
