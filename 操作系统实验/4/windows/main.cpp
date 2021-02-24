#include <windows.h>
#include <iostream>

#define BUF_SIZE 256

using namespace std;

//打印文件树型目录
void PrintFilename(char *name, int depth)
{
	for (int i = 0; i < depth * 4; i++)
	{
		cout << " ";
	}
	cout << name << endl;
}

//复制文件
void CopyFile(char src[MAX_PATH], char dest[MAX_PATH], int depth)
{
	WIN32_FIND_DATA fd;
	HANDLE fileHandle = FindFirstFile(src, &fd);
	PrintFilename(fd.cFileName, depth); //打印文件名
	HANDLE srcHandle = CreateFile(src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE destHandle = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//复制过程中的缓冲区
	char *buffer = new char[BUF_SIZE];
	memset(buffer, 0, sizeof(buffer));

	//读和写的字节数
	DWORD readIn, readOut;

	//以缓冲区大小复制
	//读文件
	while (ReadFile(srcHandle, buffer, BUF_SIZE, &readIn, NULL) && readIn > 0)
	{
		//写文件
		WriteFile(destHandle, buffer, readIn, &readOut, NULL);

		//出现错误
		if (readIn != readOut)
		{
			cout << "Error: " << GetLastError() << endl;
			return;
		}
	}

	SetFileTime(destHandle, &fd.ftCreationTime, &fd.ftLastAccessTime, &fd.ftLastWriteTime);

	//关闭句柄
	CloseHandle(srcHandle);
	CloseHandle(destHandle);
}

//复制文件夹
void CopyDirectory(char src[MAX_PATH], char dest[MAX_PATH], int depth)
{
	WIN32_FIND_DATA fd;

	//存储源路径和目标路径
	char source[MAX_PATH];
	char target[MAX_PATH];

	strcpy(source, src);
	strcpy(target, dest);
	//第一个*不指定文件名
	//第二个*不指定文件类型
	strcat(source, "\\*.*");
	strcat(target, "\\");

	HANDLE srcHandle = FindFirstFile(source, &fd);
	if (srcHandle != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(srcHandle, &fd) != 0)
		{
			//防止多文件夹错误,请理缓存
			//当前文件源路径
			strcpy(source, src);
			strcat(source, "\\");
			strcat(source, fd.cFileName);
			//当前文件目标路径
			strcpy(target, dest);
			strcat(target, "\\");
			strcat(target, fd.cFileName);

			//复制文件夹
			if (fd.dwFileAttributes == 16) //目标为文件夹时值为FILE_ATTRIBUTE_DIRECTORY(16)
			{
				//只复制当前文件夹下的子文件夹
				if ((strcmp(fd.cFileName, ".") != 0) && (strcmp(fd.cFileName, "..") != 0))
				{
					PrintFilename(fd.cFileName, depth); //打印文件名
					//创建文件夹并复制
					CreateDirectory(target, NULL);
					CopyDirectory(source, target, depth + 1);
					//修改时间信息
					HANDLE destHandle = CreateFile(dest, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					SetFileTime(destHandle, &fd.ftCreationTime, &fd.ftLastAccessTime, &fd.ftLastWriteTime);
				}
			}
			//复制文件
			else //目标为文档时的值为FILE_ATTRIBUTE_ARCHIVE(32)
			{
				//复制文件
				CopyFile(source, target, depth);
			}
		}
	}
	else
	{
		cerr << "Error: " << GetLastError() << endl;
		return;
	}
}

int main(int argc, char *argv[])
{
	// 参数判断
	if (argc != 3)
	{
		cout << "Error:input as"
			 << " src"
			 << " dest" << endl;
		return -1;
	}

	char src[MAX_PATH];
	char dest[MAX_PATH];

	// 第一个参数：源文件夹
	sprintf(src, "%s", argv[1]);
	// 第二个参数：目标文件夹
	sprintf(dest, "%s", argv[2]);

	WIN32_FIND_DATA fd, srcInfo;

	// 判断源文件夹是否存在
	HANDLE srcHandle = FindFirstFile(src, &fd);
	if (srcHandle == INVALID_HANDLE_VALUE)
	{
		cout << "Error Source directory or file don't exist." << endl;
		return -1;
	}
	else
	{
		srcInfo = fd; //保存源文件信息
	}
	//判断拷贝的是文件
	if (fd.dwFileAttributes != 16)
	{
		cout << "-----copying file-----" << endl
			 << endl;
		CopyFile(src, dest, 0);
	}
	else
	{
		//如果dest文件夹不存在则创建目标文件夹
		HANDLE destHandle = FindFirstFile(dest, &fd);
		if (destHandle == INVALID_HANDLE_VALUE)
		{
			CreateDirectory(dest, NULL);
			HANDLE destHandle = CreateFile(dest, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			//修改时间信息
			SetFileTime(destHandle, &srcInfo.ftCreationTime, &srcInfo.ftLastAccessTime, &srcInfo.ftLastWriteTime);
		}

		cout << "-----copying file list-----" << endl
			 << endl;
		cout << src << endl;
		//递归复制文件夹
		CopyDirectory(src, dest, 1);
	}

	//拷贝完成
	cout << endl
		 << "-----copy finished-----" << endl;

	return 0;
}
