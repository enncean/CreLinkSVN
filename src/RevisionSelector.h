#pragma once

#include <Windows.h>

/**
 * Revision選択ダイアログを生成する。
 *
 * @param hInstance		インスタンスハンドル
 * @return IDOK or IDCANCEL
 */
INT_PTR SelectorWindow(HINSTANCE hInstance);

/**
 * Revision選択ダイアログで選択したRevisionを取得する。<br>
 * SelectorWindow関数が IDOK を返した場合のみ取得可能。
 *
 * @return 選択したRevision。(未選択の場合は0)
 */
int GetSelectedRevision();