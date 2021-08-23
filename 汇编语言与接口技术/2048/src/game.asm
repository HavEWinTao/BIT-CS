.386
.model flat
option casemap: none

include kernel32.inc

.data 
transition		sdword	100 dup(0)
seedList        dword   2,2,2,2,2,2,2,2,4,4

.code
rand            proc  stdcall first:dword, second:dword
                invoke GetTickCount ; 取得随机数种子,指从操作系统启动所经过（elapsed）的毫秒数，返回值是DWORD
                mov ecx, 23         
                mul ecx            
                add eax, 7          
                mov ecx, second     ;second是上限，first是下限
                sub ecx, first      ; ecx = 上限 - 下限
                inc ecx             ; 得到了范围
                xor edx, edx        
                div ecx            
                add edx, first      ; 修正产生的随机数的范围
                mov eax, edx        ; eax即随机数
                ret
rand            endp

getRand         proc     C array:dword,n:sdword 
            L1:   
                mov      ebx,n
		        mov      eax,n
		        mul      ebx
		        mov      edx,eax
                dec      edx                          ;计算n*n-1

                invoke   rand,0,edx

                mov      esi,eax   
                mov      ecx,array
                mov      eax,dword ptr[ecx][esi*4]
                cmp      eax,0                       ;判断data[index]是否为0
                jz       L2                          ;为0跳出循环
                jmp      L1                          ;否则继续搜索

            L2:
                invoke   rand,0,9                    ;获取初始值数组的随机索引    
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
                xor      esi,esi                   ;数组偏移量
            L1: 
                mov     dword ptr[eax][esi*4],0     ;按双字
                mov     ebx,eax
                inc     esi                         ;偏移量+1
                loop    L1
				invoke  getRand,array,n
				invoke  getRand,array,n
			    ret
initGame        endp

Address_	proc	C array:dword,n:sdword,row:sdword,column:sdword
				local	address,row_length:dword
				push	ebx						;保存原始信息
				push	esi
				push    edi

				mov		eax,n					;计算行的长度
				mov		ebx,4
				mul		ebx
				mov		row_length,eax

				mov		eax,array				;将首地址保存到局部变量address中
				mov		address,eax

				mov		eax,row_length
				mov		ebx,row
				mul		ebx
				add		address,eax

				mov		eax,4
				mov		ebx,column
				mul		ebx
				add		address,eax

				mov		eax,address				;eax中保存数组对应位置的值，eax为函数返回结果

				pop		edi						;恢复原始信息
				pop		esi
				pop		ebx						
				ret								
Address_	endp

Address_calculate	proc	C array:dword,n:sdword,row:sdword,column:sdword
					
					push	ebx						;保存原始信息
					push	esi
					push    edi

					invoke	Address_,array,n,row,column
					mov		eax,[eax]

					pop		edi						;恢复原始信息
					pop		esi
					pop		ebx						
					ret								
Address_calculate	endp

rotate			proc	C array:dword,n:sdword
				local	row:sdword,column:sdword
				push	ebx						;保存原始信息
				push	esi
				push    edi

				xor		eax,eax 
				mov		row,eax
				mov		column,eax

				;将源数组数据保存
				mov		ecx,n					;外层循环n次
		i1000:	
				push	ecx						;压入堆栈，方便在列循环时使用
				mov		ecx,n					;内层循环n次

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

				mov		eax,row					;行偏移加一
				inc		eax
				mov		row,eax
				loop	i2000

				mov		eax,column				;列偏移加一
				inc		eax
				mov		column,eax
				pop		ecx						
				loop	i1000

				;用旋转后的数据替换原数组
				mov		ecx,n					;外层循环n次

				xor		eax,eax 
				mov		column,eax

		i3000:	
				push	ecx						;压入堆栈，方便在列循环时使用
				mov		ecx,n					;内层循环n次

				xor		eax,eax 
				mov		row,eax
		i4000:
				invoke	Address_calculate,offset transition,n,row,column
				mov		edx,eax
				
				push	edx
				invoke	Address_,array,n,row,column
				pop		edx
				mov		[eax],edx

				mov		eax,row					;行偏移加一
				inc		eax
				mov		row,eax
				loop	i4000

				mov		eax,column				;列偏移加一
				inc		eax
				mov		column,eax
				pop		ecx						
				loop	i3000

				pop		edi						;恢复原始信息
				pop		esi
				pop		ebx						
				ret			
rotate			endp

checkGameOver	proc	C array:dword,n:sdword
				local	address,val:dword
				
				push	ebx						;保存原始信息
				push	esi
				push    edi

				mov		eax,n
				mov		ebx,4
				mul		ebx
				mov		val,eax

				;判断有无空位零
				mov		ecx,n					;外层循环n次
				xor		ebx,ebx					;将ebx置零，表示行号
		i10:	
				push	ecx						;压入堆栈，方便在列循环时使用
				mov		ecx,n					;内层循环n次
				xor		edx,edx					;edx作为列数进行计算
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

				inc		edx						;列偏移加一
				loop	i20

				inc		ebx						;行偏移加一
				pop		ecx						
				loop	i10

				;判断各行是否可加
				mov		ecx,n					;外层循环n次
				xor		ebx,ebx					;将ebx置零，表示行号
		i30:	
				push	ecx						;压入堆栈，方便在列循环时使用
				mov		ecx,n					;内层循环n-1次
				dec		ecx
				xor		edx,edx					;edx作为列数进行计算
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
				inc		edx						;列偏移加一
				loop	i40

				inc		ebx						;行偏移加一
				pop		ecx						
				loop	i30
				
				;判断各列是否可加
				mov		ecx,n					;外层循环n次
				xor		ebx,ebx					;将ebx置零，表示列号
		i50:	
				push	ecx						;压入堆栈，方便在列循环时使用
				mov		ecx,n					;内层循环n-1次
				dec		ecx
				xor		edx,edx					;edx作为行数进行计算
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
				inc		edx						;列偏移加一
				loop	i60

				inc		ebx						;行偏移加一
				pop		ecx						
				loop	i50


				MOV		eax,1					;return 1;
				jmp		i90
		i80:
				mov		eax,0					;return 0;
		i90:
				pop		edi						;恢复原始信息
				pop		esi
				pop		ebx						
				ret								
checkGameOver	endp

moveUp			proc	C array:dword,n:sdword
				local	row,column,idx,isChange:dword
				
				push	ebx						;保存原始信息
				push	esi
				push    edi

				xor		eax,eax					;int isChange = 0;
				mov		row,eax
				mov		column,eax
				mov		isChange,eax

				;先累加
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

				;判断是否可加，能加则加
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
				mov		eax,row					;行偏移加一
				inc		eax				
				mov		row,eax

				jmp		i200

		end1_1:
				mov		eax,column				;列偏移加一
				inc		eax
				mov		column,eax
				
				jmp		i100

		end1:
				;累加后移动
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
				mov		eax,row					;行偏移加一
				inc		eax				
				mov		row,eax

				jmp		i500
		end2_1:
				mov		eax,column				;列偏移加一
				inc		eax
				mov		column,eax
				pop		ecx			
				
				jmp		i400

		end2:
				;成功移动之后需要随机产生一个数
				mov		eax,isChange
				cmp		eax,1
				jnz		func_end

				invoke	getRand,array,n

		func_end:
				pop		edi						;恢复原始信息
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