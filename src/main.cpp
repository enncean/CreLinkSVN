#include <filesystem>
#include <Windows.h>

#include "CreLinkCore.h"
#include "RevisionSelector.h"

enum class ApplyArgsResult
{
	SUCCESS,
	PATH_NOT_SPECIFIED,
	FILE_NOT_FOUND,
	FILE_ACCESS_ERROR
};

ApplyArgsResult ApplyArgs(CreLinkCore& coreObj);
bool SetClipboardText(const std::string& text);

constexpr TCHAR APP_TITLE[] = TEXT("CreLinkSVN");

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!CreLinkCore::CheckSubversionValid())
	{
		MessageBox(nullptr,
			TEXT("\"svn\" command is not available."),
			APP_TITLE, MB_OK | MB_ICONERROR);
		return -1;
	}
	
	CreLinkCore coreObj;
	// Parse arguments.
	{
		const ApplyArgsResult result = ApplyArgs(coreObj);
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
			}
			return -1;
		}
	}

	// Read repository and check file.
	{
		const CreLinkCore::ReadRepositoryResult result = coreObj.ReadRepository();
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
			}
			return -1;
		}
	}

	const INT_PTR selector_ret = SelectorWindow(hInstance);
	if (selector_ret == IDOK)
	{
		std::string out = coreObj.GenerateURLWithRev(GetSelectedRevision());
		const bool result = SetClipboardText(out);

		if (result)
		{
			std::string result_msg("Path \"" + out + "\" was copied to clipboard.");
			MessageBox(nullptr, std::wstring(result_msg.begin(), result_msg.end()).c_str(), APP_TITLE, MB_OK);
		}
	}
	else
	{
		MessageBox(nullptr, TEXT("Revision selector canceled."), TEXT(""), MB_OK);
	}
	
	return 0;
}

constexpr int ARG_TARGET_PATH = 1;
ApplyArgsResult ApplyArgs(CreLinkCore& coreObj)
{
	ApplyArgsResult ret = ApplyArgsResult::SUCCESS;

	if (__argc > ARG_TARGET_PATH)
	{
		const std::string path(__argv[ARG_TARGET_PATH]);
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
