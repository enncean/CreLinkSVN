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
CreLinkSVN.exe [対象ファイル]
```
|引数|詳細|
|---|---|
|対象ファイル|**必須**<br>URLを生成する対象のパスを指定する。<br>対象はSubversionでバージョン管理されているファイルでなければならない。|
