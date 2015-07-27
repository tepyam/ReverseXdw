/* �y�[�W���t���ɂ���v���O����

�@�\:
  DocuWorks�����̃y�[�W�𔽓]����B
�g�p���@:
  ReverseXdw �t�@�C����

  �t�@�C����: DocuWorks�����̃t�@�C����
*/

#include <stdio.h>
#include <windows.h>

#include <xdw_api.h>

void print_error(int code)
{
	switch (code) {
	case XDW_E_NOT_INSTALLED:
		fprintf(stderr, "DocuWorks���C���X�g�[������Ă��܂���B");
		break;
	case XDW_E_FILE_NOT_FOUND:
		fprintf(stderr, "�w�肳�ꂽ�t�@�C����������܂���B");
		break;
	case XDW_E_ACCESSDENIED:
	case XDW_E_INVALID_NAME:
	case XDW_E_BAD_NETPATH:
		fprintf(stderr, "�w�肳�ꂽ�t�@�C�����J�����Ƃ��ł��܂���B");
		break;
	case XDW_E_BAD_FORMAT:
		fprintf(stderr, "�w�肳�ꂽ�t�@�C���͐������t�H�[�}�b�g�ł͂���܂���B");
		break;
	case XDW_E_INVALID_ACCESS:
		fprintf(stderr, "�w�肳�ꂽ��������錠��������܂���B");
		break;
	case XDW_E_CANCELED:
		fprintf(stderr, "�L�����Z������܂����B");
		break;
	default:
		fprintf(stderr, "�G���[���������܂����B�R�[�h�F0x%X", code);
		break;
	}
}

int main(int argc, char** argv)
{
	int api_result = 0;

	if (argc != 2) {
		fprintf(stderr, "�g�p���@: ReverseXdw �����t�@�C����");
		return 0;
	}

	char in_path[_MAX_PATH];
	_fullpath(in_path, argv[1], _MAX_PATH);


	//��Ɨp�t�H���_���쐬
	CreateDirectory("tmp", NULL);	
	

	// �����n���h�����J��
	XDW_DOCUMENT_HANDLE h = NULL;
	XDW_OPEN_MODE_EX mode = {
		sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_UPDATE, XDW_AUTH_NODIALOGUE };
	api_result = XDW_OpenDocumentHandle(in_path, &h, (XDW_OPEN_MODE*)&mode);
	if (api_result < 0) {
		print_error(api_result);
		return 0;
	}

	// XDW_GetDocumentInformation��p���đ��y�[�W���𓾂�
	XDW_DOCUMENT_INFO info = { sizeof(XDW_DOCUMENT_INFO), 0 };
	XDW_GetDocumentInformation(h, &info);
	int last_page = info.nPages;

	//���������y�[�W��xdw�t�@�C���̃p�X������z��pages��p�ӂ���
	char ** pages;
	pages = new char*[last_page];
	for(int i=0; i<last_page; i++){
		pages[i] = new char[_MAX_PATH];
	}

	//�y�[�W�𕪊�����tmp���ɕۑ�����
	char filename[15];
	char out_path[_MAX_PATH];	

	printf("�y�[�W�𕪊����c\n");
	for (int i = 1; i <= last_page; i++) {		
		sprintf_s(filename, "tmp\\%d.xdw",i);

		_fullpath(out_path, filename, _MAX_PATH);

		strcpy(pages[i-1], out_path);

		api_result = XDW_GetPage(h,i,out_path, NULL);
		if (api_result < 0) {
			print_error(api_result);
		}
	}

	//�y�[�W�����̃y�[�W����폜����
	printf("�y�[�W���폜���c\n");
	for(int i=last_page; i>=1; i--){
		api_result = XDW_DeletePage(h, i, NULL);
		if (api_result < 0) {
			print_error(api_result);
		}
	}
	// �ύX���t�@�C���ɔ��f����
	api_result = XDW_SaveDocument(h, NULL);

	//���������y�[�W�����Ԃɐ擪�ɓ���Ă���
	printf("�y�[�W��}�����c\n");
	for(int i=0; i<last_page;i++){
		api_result = XDW_InsertDocument(h, 1, pages[i], NULL);
		if (api_result < 0) {
			print_error(api_result);
		}
	}

	//���������y�[�W�Ɣz��̍폜
	for(int i=0; i<last_page; i++){
		DeleteFile(pages[i]);
		delete[] pages[i];
		pages[i] = 0;
	}
	delete[] pages;

	// �ύX���t�@�C���ɔ��f����
	api_result = XDW_SaveDocument(h, NULL);

	// �����n���h�������
	XDW_CloseDocumentHandle(h, NULL);

	// �I������
	XDW_Finalize(NULL);
	
	printf("�������܂����B");

	if (api_result < 0) return 0;

	return 1;
}
