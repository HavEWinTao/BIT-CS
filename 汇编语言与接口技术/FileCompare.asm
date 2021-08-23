.386
.model flat,stdcall
option casemap:none

include	msvcrt.inc
includelib	msvcrt.lib
include	kernel32.inc
includelib	kernel32.lib
include	user32.inc
includelib	user32.lib
include	windows.inc
includelib	windows.inc

;声明函数
sprintf		proto	C :dword, :dword, :vararg
fgets		proto	C :dword, :dword, :dword
fopen		proto	C :dword, :dword
fclose		proto	C :dword
strcmp		proto	C :dword, :dword
strlen		proto	C :dword
strcat		proto	C :dword, :dword

;数据段
.data
;文件1
file_path1		BYTE	64	DUP(?)
fp1				DWORD	?
buf1			BYTE	1024 DUP(0)
;文件2
file_path2		BYTE	256	DUP(?)
fp2				DWORD	?
buf2			BYTE	1024 DUP(0)
;对比文件
index			DWORD	0	;显示行号
flag			DWORD	0	;用来标记文件是否有不相同的
mode			BYTE	'rb',0	;必须定义成字节型的
str1			BYTE	'No.%d line is different',0ah,0	;内容不同的行
str2			BYTE	'The content of the two files is same.',0ah,0	;文件内容相同
str3			BYTE	'Can not open the file.',0ah,0	;无法打开文件
;输出信息
outInfo			BYTE	2000 DUP(0)
;主窗口信息
hdExe			DWORD	?	;应用程序的句柄
hdWindow		DWORD	?	;窗口的句柄
winClassName	BYTE	'MyClass',0
winCaptionName	BYTE	'Compare File',0
;窗口内文本编辑框和按钮
button			BYTE	'button',0
buttonContext	BYTE	'Compare files',0

edit			BYTE	'edit',0
hdEdit1			HWND	?
hdEdit2			HWND	?

wlabel			BYTE	'static',0
label1		BYTE	'FILE1 path:',0
label2		BYTE	'FILE2 path:',0
;MessageBox标题
szTitle			BYTE	'Output',0

;代码段
.code
CompareFile	proc
	local	itemBuf[1000] :BYTE
OpenFiles:
	invoke	fopen,addr file_path1,offset mode
	mov		fp1,eax	;函数返回值在eax
	cmp		fp1,0
	je		NotOpen
	invoke	fopen,addr file_path2,offset mode
	mov		fp2,eax
	cmp		fp2,0
	je		NotOpen
ReadFiles:
	xor		eax,eax
	mov		buf1,al
	mov		buf2,al
	invoke	fgets,offset buf1,1024,fp1
	push	eax
	invoke	fgets,offset buf2,1024,fp2
	push	eax
	inc		index	;行号+1
	
EnterLine:
	invoke	strlen,offset buf1
	sub		eax,2					
	.if buf1[eax]  == 0dh
		xor	ebx,ebx
		mov buf1[eax],bl
	.endif
	invoke	strlen,offset buf2
	sub		eax,2
	.if buf2[eax]  == 0dh
		xor	ebx,ebx
		mov buf2[eax],bl
	.endif

Compare:
	invoke	strcmp,offset buf1,offset buf2
	cmp		eax,0
	jnz		NotSame
JudgeLine:	
	pop		eax
	pop		ebx	;此处需比较两个文件是否均读到了头，因为两个文件行数可以不同
	cmp		eax,ebx	;文件均读完后fgets返回值为0
	je		FileEnd
	jmp		ReadFiles	
NotSame:
	inc		flag
	invoke	sprintf,addr itemBuf,offset	str1,index
	INVOKE	strcat,	addr outInfo,addr	itemBuf
	jmp		JudgeLine
FileEnd:	
	mov		eax,flag
	cmp 	eax,0
	jnz		CloseFile
NotOpen:
	invoke	sprintf,addr itemBuf,offset	str3
	invoke	strcat,	addr outInfo,addr	itemBuf
	ret
CloseFile:
	invoke	fclose,fp1
	invoke	fclose,fp2
	ret
CompareFile	endp


windowFun	proc	stdcall hWnd, uMsg, wParam, lParam   ;窗口过程
	local	structPs :PAINTSTRUCT
	local	hDc
	
	mov		eax,uMsg

	.if		eax==WM_PAINT		
		invoke	BeginPaint,hWnd,addr structPs
		mov		hDc,eax
		invoke	EndPaint,hWnd,addr structPs
				
	.elseif	eax==WM_CLOSE	;窗口关闭信息
		invoke	DestroyWindow,hdWindow
		invoke	PostQuitMessage,NULL

	.elseif	eax==WM_CREATE    ;创建窗口
		;两个文本编辑框
		invoke	CreateWindowEx,	WS_EX_CLIENTEDGE,offset edit,NULL,\
								WS_CHILD or WS_VISIBLE or WS_BORDER or ES_LEFT or ES_AUTOHSCROLL,\
								100,10,300,30,\
								hWnd,1,hdExe,NULL   
		mov		hdEdit1,eax							
		invoke	CreateWindowEx,	WS_EX_CLIENTEDGE,offset edit, NULL,\
								WS_CHILD or WS_VISIBLE or WS_BORDER or ES_LEFT or ES_AUTOHSCROLL,\
								100,60,300,30,\
								hWnd,2,hdExe,NULL 
		mov		hdEdit2,eax 
		;2个label
		invoke	CreateWindowEx,	NULL,offset wlabel,offset label1,\
								WS_CHILD or WS_VISIBLE,\
								10,15,90,30,\
								hWnd,3,hdExe,NULL
		invoke	CreateWindowEx,	NULL,offset wlabel,offset label2,\
								WS_CHILD or WS_VISIBLE,\
								10,65,90,30,\
								hWnd,4,hdExe,NULL
		;1个button
		invoke	CreateWindowEx,	NULL,offset button,offset buttonContext,\
								WS_CHILD or WS_VISIBLE,\
								250,100, 150, 45,\
								hWnd,5,hdExe,NULL   
	.elseif	eax==WM_COMMAND
		mov eax,wParam  ;其中参数wParam里存的是句柄，如果点击了一个按钮，则wParam是那个按钮的句柄
		.if	eax==5
			invoke	GetWindowText,hdEdit1,addr file_path1,512
			invoke	GetWindowText,hdEdit2,addr file_path2,512
			invoke	CompareFile
			.if	flag==0	;相同
				invoke	MessageBox,hWnd,offset str2,offset szTitle,MB_OK
			.else	;输出不同的行数
				invoke	MessageBox,hWnd,offset outInfo,offset szTitle,MB_OK
			.endif
		.endif

	.else
		invoke	DefWindowProc,hWnd,uMsg,wParam,lParam
		ret
	.endif
	xor	eax,eax
	ret
windowFun	endp


mainWindow	proc
	local	wc	:WNDCLASSEX	;定义一个WNDCLASSEX类型的结构变量——窗口进程
	local	msg	:MSG	;定义用于消息传递的MSG类型变量
				
	invoke	GetModuleHandle,NULL	;得到应用程序的句柄
	mov		hdExe,eax
	
	invoke	RtlZeroMemory,addr wc,sizeof wc	;将structWndClaa初始化为零
	invoke	LoadCursor,0,IDC_ARROW	;载入指定的光标资源
	mov		wc.hCursor,eax
	push	hdExe
	pop		wc.hInstance
	mov		wc.cbSize,sizeof WNDCLASSEX
	mov		wc.style,CS_HREDRAW	OR	CS_VREDRAW
	mov		wc.lpfnWndProc,	offset	windowFun	;指定该窗口程序的窗口过程是_ProcWinMain
	mov		wc.hbrBackground,COLOR_WINDOW
	mov		wc.lpszClassName,offset	winClassName
	invoke	RegisterClassEx,addr	wc	;先填写WNDCLASSEX的结构再注册
	;创建窗口
	INVOKE	CreateWindowEx,WS_EX_CLIENTEDGE,offset winClassName,offset winCaptionName,WS_OVERLAPPEDWINDOW,500,200,460,200,NULL,NULL,hdExe,NULL
	mov		hdWindow,eax
	;启动窗口
	invoke	ShowWindow,hdWindow,SW_SHOWNORMAL	;显示窗口
	invoke	UpdateWindow,hdWindow	;刷新窗口客户区

	.while	TRUE	;进入无限循环的消息获取和处理
		invoke	GetMessage,addr	msg,NULL,0,0
		.break	.if	eax==0	;若退出循环,eax置为零
		invoke	TranslateMessage,addr msg	;将基于键盘扫描码的按键信息转换成对应的ASCII码，若消息非键盘输入,这步跳过
		invoke	DispatchMessage, addr msg	;通过该窗口的窗口过程处理消息
	.endw
	ret
mainWindow	endp

main	proc
	call	mainWindow
	invoke	ExitProcess,NULL
	ret
main	endp
end	main
