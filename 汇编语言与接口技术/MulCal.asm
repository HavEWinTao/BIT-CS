.386
.model flat, stdcall
option casemap:none

include	msvcrt.inc
includelib msvcrt.lib

;声明C函数
scanf	proto c :dword,:vararg	
printf	proto c :dword,:vararg
strlen	proto c :dword,:vararg

;数据区
.data
	sysPause BYTE		'pause',0
	num1	BYTE	100 dup(?)	;数字1
	num2	BYTE	100 dup(?)	;数字2
	mid		BYTE	20000 dup(0)
	ans		BYTE	200 dup(?)	;ans以ASCII码存放
	len1	DWORD	?	;数字1的长度
	len2	DWORD	?	;数字2的长度
	len		DWORD	?	;两个数字的总长度
	
	;输出字符串
	info	BYTE	'The function of this program is to calculate the result of the multiplication of two numbers',0ah,0
	str1	BYTE	'input first number: ',0
	str2	BYTE	'input second number: ',0
    output 	BYTE 'the ans is: %s',0ah,0
	;输入字符串
	input  	BYTE '%s',0

;代码区
.code
;输入
inputFun	proc
	invoke	printf, offset info
	invoke 	printf, offset str1
    invoke  scanf,offset input,offset num1
    invoke 	printf, offset str2
    invoke  scanf,offset input,offset num2
	ret
inputFun	endp

start:
	
    invoke	inputFun
    	
	invoke	strlen,offset num1	;返回值保存在EAX中
	mov 	len1,eax
	mov		len,eax	;求num1与num2两个数组的长度
	xor		eax,eax	;EAX清零
	invoke 	strlen,offset num2
	mov 	len2,eax
	add		len,eax
	
;把两个字符串数组的字符转化为数字	
	mov		eax,-1	
convert_num1:
	inc 	eax
	cmp		eax,len1			
	je		num1_break	;相等转移	
	sub		num1[eax],30h	; -‘0’30h是'0'的ASCII值	
	jmp		convert_num1
num1_break:	

	mov		eax,-1
convert_num2:	
	inc 	eax
	cmp		eax,len2	
	je		num2_break
	sub		num2[eax],30h	
	jmp		convert_num2
num2_break:

;乘法运算
;对edi进行初始化,ecx表示i,edi表示j
	mov 	ecx,-1
for1:	;for(i=0;i<len2;i++)
	inc 	ecx	
	cmp 	ecx,len2
	je  	for1_break	;相等转移
	mov 	edi,-1			 
for2:	;for(j=0;j<len1;j++)
	inc 	edi
	cmp		edi,len1
	jz		for1	;跳出第二重循环
	mov 	esi,edi
	add		esi,ecx	;有bug
	xor		eax,eax
	mov 	al,num1[edi]	
	mul 	num2[ecx]	;结果在AX中
	push	ecx
	push	eax
	mov		ax,cx
	mov		ebx,100	;MaxLen
	mul		bx
	mov		cx,dx
	shl		ecx,16	;逻辑左移
	mov		cx,ax;此段代码为了让数组不会被重叠
	pop		eax
	mov 	mid[ecx][esi],al	;保存每位乘法的运算结果
	pop		ecx
	jmp 	for2
for1_break:
	
	xor 	edx,edx
	xor		ecx,ecx
	mov 	esi,len
	sub		esi,1	;esi+1作为ans的下标
Index:;对于ans的下标
	dec		esi	
	cmp		esi,0
	jl		Index0	
	mov		ecx,edx
	mov		ebx,len2		;对i进行初始化
	
IndexSum:
	dec		ebx
	cmp		ebx,0
	jl		Assign
	push	ebx
	mov		ax,bx
	mov		ebx,100	;MaxLen
	mul		bx
	mov		bx,dx
	shl		ebx,16
	mov		bx,ax
			
	mov		al,mid[ebx][esi]
	movzx	ebx,al
	add		ecx,ebx
	pop		ebx
	jmp		IndexSum	
			
Assign:
	mov	ax,cx
	shr	ecx,16	;逻辑右移			;将ecx高16位放入dx
	mov	dx,cx
	mov	bx,10			;除法运算,此处可扩展为更高位数的除法
	div	bx
	add	dx,30h	
	mov	ans[esi][1],dl	
	mov	dx,ax
	jmp	Index
	
Index0:
	add	dl,30h
	mov	ans[0],dl

ShowAns:
	xor	ebx,ebx
	;找到ans第一个不为0的位置
	dec	len
	.while	ans[ebx] == 30h
		inc	ebx		
		.if	ebx == len
			.break
		.endif
	.endw	
	;输出结果
	invoke 	printf,offset output,addr ans[ebx]
	invoke crt_system,addr sysPause
	ret
end start