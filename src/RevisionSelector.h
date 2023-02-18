#pragma once

#include <Windows.h>

#include "SVNOut.h"

/**
 * Revision選択ダイアログを生成する。
 *
 * @param hInstance		インスタンスハンドル
 * @param commit_log	対象ファイルのコミットログ
 * @return IDOK or IDCANCEL
 */
INT_PTR SelectorWindow(HINSTANCE hInstance, std::vector<SVNLog::LogItem>& commit_log);

/**
 * Revision選択ダイアログで選択したRevisionを取得する。<br>
 * SelectorWindow関数が IDOK を返した場合のみ取得可能。
 *
 * @return 選択したRevision。(未選択の場合は0)
 */
int GetSelectedRevision();