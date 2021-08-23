.386
.model flat
option casemap: none

include kernel32.inc

.data 
transition		sdword	100 dup(0)
seedList        dword   2,2,2,2,2,2,2,2,4,4

.code
rand            proc  stdcall first:dword, second:dword
                invoke GetTickCount ; ȡ�����������,ָ�Ӳ���ϵͳ������������elapsed���ĺ�����������ֵ��DWORD
                mov ecx, 23         
                mul ecx            
                add eax, 7          
                mov ecx, second     ;second�����ޣ�first������
                sub ecx, first      ; ecx = ���� - ����
                inc ecx             ; �õ��˷�Χ
                xor edx, edx        
                div ecx            
                add edx, first      ; ����������������ķ�Χ
                mov eax, edx        ; eax�������
                ret
rand            endp

getRand         proc     C array:dword,n:sdword 
            L1:   
                mov      ebx,n
		        mov      eax,n
		        mul      ebx
		        mov      edx,eax
                dec      edx                          ;����n*n-1

                invoke   rand,0,edx

                mov      esi,eax   
                mov      ecx,array
                mov      eax,dword ptr[ecx][esi*4]
                cmp      eax,0                       ;�ж�data[index]�Ƿ�Ϊ0
                jz       L2                          ;Ϊ0����ѭ��
                jmp      L1                          ;�����������

            L2:
                invoke   rand,0,9                    ;��ȡ��ʼֵ������������    
                mov      edx,eax                     
                mov      eax,dword ptr seedList[edx*4]    
                mov      ecx,array
                mov      dword ptr[ecx][esi*4],eax            
		        ret
getRand         endp

initGame        proc    C array:dword,n:sdword 
                mov      ebx,n
		        mov      eax,n
		        mul      ebx
                mov      ecx,eax
                mov      eax,array
                xor      esi,esi                   ;����ƫ����
            L1: 
                mov     dword ptr[eax][esi*4],0     ;��˫��
                mov     ebx,eax
                inc     esi                         ;ƫ����+1
                loop    L1
				invoke  getRand,array,n
				invoke  getRand,array,n
			    ret
initGame        endp

Address_	proc	C array:dword,n:sdword,row:sdword,column:sdword
				local	address,row_length:dword
				push	ebx						;����ԭʼ��Ϣ
				push	esi
				push    edi

				mov		eax,n					;�����еĳ���
				mov		ebx,4
				mul		ebx
				mov		row_length,eax

				mov		eax,array				;���׵�ַ���浽�ֲ�����address��
				mov		address,eax

				mov		eax,row_length
				mov		ebx,row
				mul		ebx
				add		address,eax

				mov		eax,4
				mov		ebx,column
				mul		ebx
				add		address,eax

				mov		eax,address				;eax�б��������Ӧλ�õ�ֵ��eaxΪ�������ؽ��

				pop		edi						;�ָ�ԭʼ��Ϣ
				pop		esi
				pop		ebx						
				ret								
Address_	endp

Address_calculate	proc	C array:dword,n:sdword,row:sdword,column:sdword
					
					push	ebx						;����ԭʼ��Ϣ
					push	esi
					push    edi

					invoke	Address_,array,n,row,column
					mov		eax,[eax]

					pop		edi						;�ָ�ԭʼ��Ϣ
					pop		esi
					pop		ebx						
					ret								
Address_calculate	endp

rotate			proc	C array:dword,n:sdword
				local	row:sdword,column:sdword
				push	ebx						;����ԭʼ��Ϣ
				push	esi
				push    edi

				xor		eax,eax 
				mov		row,eax
				mov		column,eax

				;��Դ�������ݱ���
				mov		ecx,n					;���ѭ��n��
		i1000:	
				push	ecx						;ѹ���ջ����������ѭ��ʱʹ��
				mov		ecx,n					;�ڲ�ѭ��n��

				xor		eax,eax 
				mov		row,eax
		i2000:
				invoke	Address_calculate,array,n,row,column
				mov		edx,eax
				
				mov		eax,n
				dec		eax
				sub		eax,row
				push	edx
				invoke	Address_,offset transition,n,column,eax
				pop		edx
				mov		[eax],edx

				mov		eax,row					;��ƫ�Ƽ�һ
				inc		eax
				mov		row,eax
				loop	i2000

				mov		eax,column				;��ƫ�Ƽ�һ
				inc		eax
				mov		column,eax
				pop		ecx						
				loop	i1000

				;����ת��������滻ԭ����
				mov		ecx,n					;���ѭ��n��

				xor		eax,eax 
				mov		column,eax

		i3000:	
				push	ecx						;ѹ���ջ����������ѭ��ʱʹ��
				mov		ecx,n					;�ڲ�ѭ��n��

				xor		eax,eax 
				mov		row,eax
		i4000:
				invoke	Address_calculate,offset transition,n,row,column
				mov		edx,eax
				
				push	edx
				invoke	Address_,array,n,row,column
				pop		edx
				mov		[eax],edx

				mov		eax,row					;��ƫ�Ƽ�һ
				inc		eax
				mov		row,eax
				loop	i4000

				mov		eax,column				;��ƫ�Ƽ�һ
				inc		eax
				mov		column,eax
				pop		ecx						
				loop	i3000

				pop		edi						;�ָ�ԭʼ��Ϣ
				pop		esi
				pop		ebx						
				ret			
rotate			endp

checkGameOver	proc	C array:dword,n:sdword
				local	address,val:dword
				
				push	ebx						;����ԭʼ��Ϣ
				push	esi
				push    edi

				mov		eax,n
				mov		ebx,4
				mul		ebx
				mov		val,eax

				;�ж����޿�λ��
				mov		ecx,n					;���ѭ��n��
				xor		ebx,ebx					;��ebx���㣬��ʾ�к�
		i10:	
				push	ecx						;ѹ���ջ����������ѭ��ʱʹ��
				mov		ecx,n					;�ڲ�ѭ��n��
				xor		edx,edx					;edx��Ϊ�������м���
		i20:
				mov		eax,array
				mov		address,eax

				mov		eax,val
				push	edx
				mul		ebx
				pop		edx
				add		address,eax

				mov		eax,4
				push	edx
				mul		edx
				pop		edx
				add		address,eax

				mov		eax,address
				mov		eax,[eax]
				cmp		eax,0
				jz		i80						;return 0;

				inc		edx						;��ƫ�Ƽ�һ
				loop	i20

				inc		ebx						;��ƫ�Ƽ�һ
				pop		ecx						
				loop	i10

				;�жϸ����Ƿ�ɼ�
				mov		ecx,n					;���ѭ��n��
				xor		ebx,ebx					;��ebx���㣬��ʾ�к�
		i30:	
				push	ecx						;ѹ���ջ����������ѭ��ʱʹ��
				mov		ecx,n					;�ڲ�ѭ��n-1��
				dec		ecx
				xor		edx,edx					;edx��Ϊ�������м���
		i40:
				mov		eax,array
				mov		address,eax

				mov		eax,val
				push	edx
				mul		ebx
				pop		edx
				add		address,eax

				mov		eax,4
				push	edx
				mul		edx
				pop		edx
				add		address,eax

				mov		eax,address
				push	edx
				mov		edx,[eax]
				mov		eax,[eax+4]

				cmp		eax,edx
				jz		i80						;return 0;

				pop		edx
				inc		edx						;��ƫ�Ƽ�һ
				loop	i40

				inc		ebx						;��ƫ�Ƽ�һ
				pop		ecx						
				loop	i30
				
				;�жϸ����Ƿ�ɼ�
				mov		ecx,n					;���ѭ��n��
				xor		ebx,ebx					;��ebx���㣬��ʾ�к�
		i50:	
				push	ecx						;ѹ���ջ����������ѭ��ʱʹ��
				mov		ecx,n					;�ڲ�ѭ��n-1��
				dec		ecx
				xor		edx,edx					;edx��Ϊ�������м���
		i60:
				mov		eax,array
				mov		address,eax

				mov		eax,4
				push	edx
				mul		ebx
				pop		edx
				add		address,eax

				mov		eax,val
				push	edx
				mul		edx
				pop		edx
				add		address,eax

				mov		eax,address
				push	edx
				mov		edx,[eax]
				add		eax,val
				mov		eax,[eax]

				cmp		eax,edx
				jz		i80						;return 0;

				pop		edx
				inc		edx						;��ƫ�Ƽ�һ
				loop	i60

				inc		ebx						;��ƫ�Ƽ�һ
				pop		ecx						
				loop	i50


				MOV		eax,1					;return 1;
				jmp		i90
		i80:
				mov		eax,0					;return 0;
		i90:
				pop		edi						;�ָ�ԭʼ��Ϣ
				pop		esi
				pop		ebx						
				ret								
checkGameOver	endp

moveUp			proc	C array:dword,n:sdword
				local	row,column,idx,isChange:dword
				
				push	ebx						;����ԭʼ��Ϣ
				push	esi
				push    edi

				xor		eax,eax					;int isChange = 0;
				mov		row,eax
				mov		column,eax
				mov		isChange,eax

				;���ۼ�
				xor		eax,eax					;y = 0
				mov		column,eax
		i100:
				mov		eax,n					;y < N?
				cmp		column,eax
				jnl		end1

				xor		eax,eax					;x  =  0
				mov		row,eax
		i200:
				mov		eax,n					;x < N - 1?
				dec		eax
				cmp		row,eax
				jnl		end1_1
				
				invoke	Address_calculate,array,n,row,column
				cmp		eax,0					;if (data[x][y] == 0)
				jz		continue2

				;�ж��Ƿ�ɼӣ��ܼ����
				mov		eax,row					;idx = x + 1
				inc		eax
				mov		idx,eax

		i300:
				mov		eax,n
				dec		eax
				mov		ebx,idx
				cmp		ebx,eax
				JG		continue2		;idx <= N - 1

				invoke	Address_calculate,array,n,idx,column	;if (data[idx][y] == 0)
				cmp		eax,0
				jz		continue3

				invoke	Address_calculate,array,n,idx,column
				mov		ebx,eax
				invoke	Address_calculate,array,n,row,column
				cmp		eax,ebx					;else if (data[idx][y] != data[x][y])
				jne		continue2

				;data[x][y] += data[idx][y];
				invoke	Address_calculate,array,n,idx,column
				mov		ebx,eax
				invoke	Address_calculate,array,n,row,column
				add		ebx,eax
				invoke	Address_,array,n,row,column
				mov		[eax],ebx

				;data[idx][y] = 0;
				invoke	Address_,array,n,idx,column
				mov		ebx,0
				mov		[eax],ebx

				mov		eax,1
				mov		isChange,eax

				jmp		continue2

		continue3:
				mov		eax,idx					;++idx
				inc		eax
				mov		idx,eax

				jmp		i300
				
		continue2:
				mov		eax,row					;��ƫ�Ƽ�һ
				inc		eax				
				mov		row,eax

				jmp		i200

		end1_1:
				mov		eax,column				;��ƫ�Ƽ�һ
				inc		eax
				mov		column,eax
				
				jmp		i100

		end1:
				;�ۼӺ��ƶ�
				xor		eax,eax					;y = 0
				mov		column,eax
		i400:
				mov		eax,n					;y < N?
				cmp		column,eax
				jnl		end2

				xor		eax,eax					;x  =  1
				inc		eax
				mov		row,eax
		i500:
				mov		eax,n					;x < N ?
				cmp		row,eax
				jnl		end2_1

				invoke	Address_calculate,array,n,row,column
				cmp		eax,0					;if (data[x][y] == 0)
				jz		continue4

				mov		eax,row					;idx = x - 1
				dec		eax
				mov		idx,eax

		while_loop:
				invoke	Address_calculate,array,n,idx,column
				cmp		eax,0
				jne		end_while
				mov		eax,idx
				cmp		eax,0
				jl		end_while

				mov		eax,idx					;--idx;
				dec		eax
				mov		idx,eax

				jmp		while_loop

		end_while:
				mov		eax,idx					;if (data[idx + 1][y] == 0)
				inc		eax
				invoke	Address_calculate,array,n,eax,column
				cmp		eax,0
				jnz		continue4

				invoke	Address_calculate,array,n,row,column	;data[idx + 1][y] = data[x][y];
				mov		ebx,eax
				mov		eax,idx
				inc		eax
				invoke	Address_,array,n,eax,column
				mov		[eax],ebx

				invoke	Address_,array,n,row,column			;data[x][y] = 0;
				mov		ebx,0
				mov		[eax],ebx

				mov		eax,1						;isChange = 1;
				mov		isChange,eax
				
		continue4:
				mov		eax,row					;��ƫ�Ƽ�һ
				inc		eax				
				mov		row,eax

				jmp		i500
		end2_1:
				mov		eax,column				;��ƫ�Ƽ�һ
				inc		eax
				mov		column,eax
				pop		ecx			
				
				jmp		i400

		end2:
				;�ɹ��ƶ�֮����Ҫ�������һ����
				mov		eax,isChange
				cmp		eax,1
				jnz		func_end

				invoke	getRand,array,n

		func_end:
				pop		edi						;�ָ�ԭʼ��Ϣ
				pop		esi
				pop		ebx	
				
				ret								
moveUp			endp

moveDown		proc	C array:dword,n:sdword
				
				invoke	rotate,array,n
				invoke	rotate,array,n
				invoke	moveUp,array,n
				invoke	rotate,array,n
				invoke	rotate,array,n

				ret
moveDown		endp

moveLeft		proc	C array:dword,n:sdword
				
				invoke	rotate,array,n
				invoke	moveUp,array,n
				invoke	rotate,array,n
				invoke	rotate,array,n
				invoke	rotate,array,n

				ret
moveLeft		endp

moveRight		proc	C array:dword,n:sdword
				
				invoke	rotate,array,n
				invoke	rotate,array,n
				invoke	rotate,array,n
				invoke	moveUp,array,n
				invoke	rotate,array,n

				ret
moveRight		endp

end