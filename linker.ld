ENTRY(kentry)
SECTIONS{
	. = 0x9030;
	.text : {
		*(.text)
	}
}
