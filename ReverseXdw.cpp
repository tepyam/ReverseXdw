/* ページを逆順にするプログラム

機能:
  DocuWorks文書のページを反転する。
使用方法:
  ReverseXdw ファイル名

  ファイル名: DocuWorks文書のファイル名
*/

#include <stdio.h>
#include <windows.h>

#include <xdw_api.h>

void print_error(int code)
{
	switch (code) {
	case XDW_E_NOT_INSTALLED:
		fprintf(stderr, "DocuWorksがインストールされていません。");
		break;
	case XDW_E_FILE_NOT_FOUND:
		fprintf(stderr, "指定されたファイルが見つかりません。");
		break;
	case XDW_E_ACCESSDENIED:
	case XDW_E_INVALID_NAME:
	case XDW_E_BAD_NETPATH:
		fprintf(stderr, "指定されたファイルを開くことができません。");
		break;
	case XDW_E_BAD_FORMAT:
		fprintf(stderr, "指定されたファイルは正しいフォーマットではありません。");
		break;
	case XDW_E_INVALID_ACCESS:
		fprintf(stderr, "指定された操作をする権利がありません。");
		break;
	case XDW_E_CANCELED:
		fprintf(stderr, "キャンセルされました。");
		break;
	default:
		fprintf(stderr, "エラーが発生しました。コード：0x%X", code);
		break;
	}
}

int main(int argc, char** argv)
{
	int api_result = 0;

	if (argc != 2) {
		fprintf(stderr, "使用方法: ReverseXdw 文書ファイル名");
		return 0;
	}

	char in_path[_MAX_PATH];
	_fullpath(in_path, argv[1], _MAX_PATH);


	//作業用フォルダを作成
	CreateDirectory("tmp", NULL);	
	

	// 文書ハンドルを開く
	XDW_DOCUMENT_HANDLE h = NULL;
	XDW_OPEN_MODE_EX mode = {
		sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_UPDATE, XDW_AUTH_NODIALOGUE };
	api_result = XDW_OpenDocumentHandle(in_path, &h, (XDW_OPEN_MODE*)&mode);
	if (api_result < 0) {
		print_error(api_result);
		return 0;
	}

	// XDW_GetDocumentInformationを用いて総ページ数を得る
	XDW_DOCUMENT_INFO info = { sizeof(XDW_DOCUMENT_INFO), 0 };
	XDW_GetDocumentInformation(h, &info);
	int last_page = info.nPages;

	//分割したページのxdwファイルのパスを入れる配列pagesを用意する
	char ** pages;
	pages = new char*[last_page];
	for(int i=0; i<last_page; i++){
		pages[i] = new char[_MAX_PATH];
	}

	//ページを分割してtmp内に保存する
	char filename[15];
	char out_path[_MAX_PATH];	

	printf("ページを分割中…\n");
	for (int i = 1; i <= last_page; i++) {		
		sprintf_s(filename, "tmp\\%d.xdw",i);

		_fullpath(out_path, filename, _MAX_PATH);

		strcpy(pages[i-1], out_path);

		api_result = XDW_GetPage(h,i,out_path, NULL);
		if (api_result < 0) {
			print_error(api_result);
		}
	}

	//ページを後ろのページから削除する
	printf("ページを削除中…\n");
	for(int i=last_page; i>=1; i--){
		api_result = XDW_DeletePage(h, i, NULL);
		if (api_result < 0) {
			print_error(api_result);
		}
	}
	// 変更をファイルに反映する
	api_result = XDW_SaveDocument(h, NULL);

	//分割したページを順番に先頭に入れていく
	printf("ページを挿入中…\n");
	for(int i=0; i<last_page;i++){
		api_result = XDW_InsertDocument(h, 1, pages[i], NULL);
		if (api_result < 0) {
			print_error(api_result);
		}
	}

	//分割したページと配列の削除
	for(int i=0; i<last_page; i++){
		DeleteFile(pages[i]);
		delete[] pages[i];
		pages[i] = 0;
	}
	delete[] pages;

	// 変更をファイルに反映する
	api_result = XDW_SaveDocument(h, NULL);

	// 文書ハンドルを閉じる
	XDW_CloseDocumentHandle(h, NULL);

	// 終了処理
	XDW_Finalize(NULL);
	
	printf("完了しました。");

	if (api_result < 0) return 0;

	return 1;
}
