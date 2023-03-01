# CreLinkSVN
SubversionのWCからRevision付きのURLを生成し、クリップボードにコピーするツール。

## Requirement

- Windows 10
- Subversionインストール済み(svn.exeにPathが通っていること)


## Installation

1. [Releases](https://github.com/enncean/CreLinkSVN/releases) から最新versionの CreLinkSVN.exe をダウンロードし、任意のディレクトリに配置する。
1. 環境変数「PATH」に CreLinkSVN.exe を配置したディレクトリのパスを追加する。


## Arguments

```
CreLinkSVN.exe [-Q] [-S] [対象ファイル]
```
|引数|詳細|
|---|---|
|-Q|正常実行時(キャンセルを含む)、処理完了メッセージを表示しない。|
|-S|実行時の各種チェックをスキップして動作を高速にする。<br>スキップするチェック項目は以下：<br>* `svn`コマンドが有効か<br>* 対象ファイルが存在し、バージョン管理対象になっているか|
|対象ファイル|**必須**<br>URLを生成する対象のパスを指定する。<br>対象はSubversionでバージョン管理されているファイルでなければならない。|
