ENTRY(kentry)
SECTIONS{
	. = 0x7E30;
	.text : {
		*(.text)
	}
}
