[BITS 32]
		MOV		AL,'A'
		CALL    2*8:0xc1a
fin:
		HLT
		JMP		fin
	