.386
.model flat,stdcall
option casemap:none

include msvcrt.inc
includelib  msvcrt.lib

include winmm.inc   ;timeGetTime����ͷ�ļ�
includelib  winmm.lib

scanf	proto c :dword,:vararg	
printf	proto c :dword,:vararg

fopen   proto C :dword,:dword
fclose  proto C :dword

;�ƶ��ļ�ָ�뵽ָ��λ��
fseek   proto C: dword,:dword,:dword  ;�ƶ��ļ�ָ�뵽ָ��λ��fseek(�ļ�ָ��*fp,ƫ����pos,�ļ�ָ�뵱ǰλ��origin)
;�õ��ļ�λ��ָ��ĵ�ǰֵ
ftell   proto C: dword  ;ftell(�ļ�ָ��*fp)

;���ļ��ж�ȡ���ݵ�������
fread   proto C: dword,:dword,:dword,:dword;fread(������*buf,size,nmemb,�ļ�ָ��*fp)	
;���ļ���д������
fwrite  proto C: dword,:dword,:dword,:dword;fwrite(������*buf,size,nmemb,�ļ�ָ��*fp)

;������
.data
    sysPause        BYTE    'pause',0
    buffer          BYTE    2048 dup (?)
    ;Դ�ļ�
    filename1       BYTE    100 dup (0)
    mode1           BYTE    'rb',0
    fp1             DWORD   0
    flag1           DWORD   0   ;����ļ��Ƿ����
    Filelength      DWORD   0
    ;Ŀ���ļ�
    filename2       BYTE    100 dup (0)
    mode2           BYTE    'wb',0
    fp2             DWORD   0
	flag2           DWORD   0

    readLength      DWORD   0   ;��ȡ�����ֽ���
    writeLength     DWORD   0
      	

    ;��ȡʱ��
    readStartTime   DWORD   0
    readEndTime     DWORD   0
    read_time       DWORD   0
    ;д��ʱ��
    writeStartTime  DWORD   0
    writeEndTime    DWORD   0
    write_time      DWORD   0
	      
    str_tip1        BYTE    '������Դ�ļ���·��:',0ah,0
	str_tip2        BYTE    '�������ļ�����·��:',0ah,0
    str_error1      BYTE    'ERROR: Դ�ļ���ʧ��',0ah,0
    str_error2      BYTE    'ERROR: ���ܴ���Ŀ���ļ�',0ah,0
    str_error3      BYTE    'ERROR: ��Ŀ���ļ�д������ʧ��',0ah, 0
	      
	      
    str_readtime    BYTE    '��ȡ�ļ����ѣ�%d����',0ah,0
    str_writetime   BYTE    'д���ļ����ѣ�%d����',0ah,0
	      
    str_filename    BYTE    '%s',0

;������
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
    mov     flag1,1 ;����ļ�������flag1=1
	    
    invoke  fopen,offset filename2,offset mode2
    mov     fp2,eax
    cmp     fp2,0
    jz      NotOpen
    mov     flag2,1 ;����ļ�������flag2=1
GetFilelength:
    invoke  fseek,fp1,0,2   ;�ļ�ָ���ƶ����ļ�ĩβ
    invoke  ftell,fp1   ;���ڻ�ȡ�ļ�1�ĳ���
    mov     Filelength,eax

ReadFile1:  ;��ȡ��ȡ�ļ�1��ʱ��
    invoke  fseek,fp1,readLength,0  ;���ļ�ָ���ƶ����ļ���ͷ
    invoke  timeGetTime ;��ʼ���ļ���ʱ��
    mov     readStartTime,eax
    invoke  fread,offset buffer,1,2048,fp1
    add     readLength,eax  ;ʵ�ʶ�ȡ�����ֽ���
    invoke  timeGetTime ;�������ļ���ʱ��
    mov     readEndTime,eax
    sub     eax,readStartTime
    add     read_time,eax   ;����ʱ���      
WriteFile2:	;��ȡд���ļ�2��ʱ��
    invoke  fseek,fp2,writeLength,0 
    invoke  timeGetTime ;��ʼд�ļ���ʱ��
    mov     writeStartTime,eax
    mov     eax,readLength
    .if eax==Filelength
        sub eax,writeLength
        invoke  fwrite,offset buffer,1,eax,fp2
    .else
        invoke  fwrite,offset buffer,1,2048,fp2
    .endif
    add     writeLength,eax ;ʵ��д����ֽ���
    invoke  timeGetTime ;����д�ļ���ʱ��
    mov     writeEndTime,eax
    sub     eax,writeStartTime
    add     write_time,eax  ;����ʱ���

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