[global switch_to]

;就是切换各寄存器的值的操作
switch_to:
		mov eax, [esp + 4]

		mov [eax + 0],  esp
		mov [eax + 4],  ebp
		mov [eax + 8],  ebx
		mov [eax + 12], esi
		mov [eax + 16], edi
		pushf
		mov [eax + 20], ecx

		mov esp, [eax+0]
		mov ebp, [eax+4]
		mov ebx, [eax+8]
		mov esi, [eax+12]
		mov edi, [eax+16]
		mov eax, [eax+20]
		push eax
		popf

		ret

