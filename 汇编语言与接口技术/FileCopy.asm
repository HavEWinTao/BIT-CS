.386
.model flat,stdcall
option casemap:none

include msvcrt.inc
includelib  msvcrt.lib

include winmm.inc   ;timeGetTime函数头文件
includelib  winmm.lib

scanf	proto c :dword,:vararg	
printf	proto c :dword,:vararg

fopen   proto C :dword,:dword
fclose  proto C :dword

;移动文件指针到指定位置
fseek   proto C: dword,:dword,:dword  ;移动文件指针到指定位置fseek(文件指针*fp,偏移量pos,文件指针当前位置origin)
;得到文件位置指针的当前值
ftell   proto C: dword  ;ftell(文件指针*fp)

;从文件中读取数据到缓冲区
fread   proto C: dword,:dword,:dword,:dword;fread(缓冲区*buf,size,nmemb,文件指针*fp)	
;向文件中写入数据
fwrite  proto C: dword,:dword,:dword,:dword;fwrite(缓冲区*buf,size,nmemb,文件指针*fp)

;数据区
.data
    sysPause        BYTE    'pause',0
    buffer          BYTE    2048 dup (?)
    ;源文件
    filename1       BYTE    100 dup (0)
    mode1           BYTE    'rb',0
    fp1             DWORD   0
    flag1           DWORD   0   ;标记文件是否存在
    Filelength      DWORD   0
    ;目标文件
    filename2       BYTE    100 dup (0)
    mode2           BYTE    'wb',0
    fp2             DWORD   0
	flag2           DWORD   0

    readLength      DWORD   0   ;读取到的字节数
    writeLength     DWORD   0
      	

    ;读取时间
    readStartTime   DWORD   0
    readEndTime     DWORD   0
    read_time       DWORD   0
    ;写入时间
    writeStartTime  DWORD   0
    writeEndTime    DWORD   0
    write_time      DWORD   0
	      
    str_tip1        BYTE    '请输入源文件的路径:',0ah,0
	str_tip2        BYTE    '请输入文件复制路径:',0ah,0
    str_error1      BYTE    'ERROR: 源文件打开失败',0ah,0
    str_error2      BYTE    'ERROR: 不能创建目标文件',0ah,0
    str_error3      BYTE    'ERROR: 向目标文件写入数据失败',0ah, 0
	      
	      
    str_readtime    BYTE    '读取文件花费：%d毫秒',0ah,0
    str_writetime   BYTE    '写入文件花费：%d毫秒',0ah,0
	      
    str_filename    BYTE    '%s',0

;代码区
.code
start:
GetFilename:
    invoke  printf,addr str_tip1
    invoke  scanf,addr str_filename,addr filename1
    invoke  printf,addr str_tip2
    invoke  scanf,addr str_filename,addr filename2
OpenFile:
    invoke  fopen,offset filename1,offset mode1
    mov     fp1,eax
    cmp     fp1,0
    jz      NotOpen
    mov     flag1,1 ;如果文件存在置flag1=1
	    
    invoke  fopen,offset filename2,offset mode2
    mov     fp2,eax
    cmp     fp2,0
    jz      NotOpen
    mov     flag2,1 ;如果文件存在置flag2=1
GetFilelength:
    invoke  fseek,fp1,0,2   ;文件指针移动到文件末尾
    invoke  ftell,fp1   ;用于获取文件1的长度
    mov     Filelength,eax

ReadFile1:  ;获取读取文件1的时间
    invoke  fseek,fp1,readLength,0  ;将文件指针移动到文件开头
    invoke  timeGetTime ;开始读文件的时间
    mov     readStartTime,eax
    invoke  fread,offset buffer,1,2048,fp1
    add     readLength,eax  ;实际读取到的字节数
    invoke  timeGetTime ;结束读文件的时间
    mov     readEndTime,eax
    sub     eax,readStartTime
    add     read_time,eax   ;计算时间差      
WriteFile2:	;获取写入文件2的时间
    invoke  fseek,fp2,writeLength,0 
    invoke  timeGetTime ;开始写文件的时间
    mov     writeStartTime,eax
    mov     eax,readLength
    .if eax==Filelength
        sub eax,writeLength
        invoke  fwrite,offset buffer,1,eax,fp2
    .else
        invoke  fwrite,offset buffer,1,2048,fp2
    .endif
    add     writeLength,eax ;实际写入的字节数
    invoke  timeGetTime ;结束写文件的时间
    mov     writeEndTime,eax
    sub     eax,writeStartTime
    add     write_time,eax  ;计算时间差

    mov     eax,readLength
    ;cmp     writeLength, eax
    ;jnz     CopyError
    .if eax<Filelength
        jmp     ReadFile1
    .endif
    jmp     CopyFinish
;CopyError:
;    invoke  printf,addr str_error3
CopyFinish:
    invoke  fclose,fp1
    invoke  fclose,fp2    
    invoke  printf,addr str_readtime,read_time 
    invoke  printf,addr str_writetime,write_time
    invoke crt_system,addr sysPause
    ret
NotOpen:
    .if flag1==0 
        invoke printf, addr str_error1
    .endif      
    .if flag2==0
        invoke printf, addr str_error2
        invoke fclose, fp1
    .endif
    invoke crt_system,addr sysPause
    ret
end start