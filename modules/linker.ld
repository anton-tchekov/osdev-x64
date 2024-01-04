OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

PHDRS
{
	null	PT_NULL	FLAGS(0) ;
	text	PT_LOAD	FLAGS((1 << 0) | (1 << 2)) ;
	rodata  PT_LOAD	FLAGS((1 << 2)) ;
	data	PT_LOAD	FLAGS((1 << 1) | (1 << 2)) ;
	dynamic PT_DYNAMIC FLAGS((1 << 1) | (1 << 2)) ;
}


SECTIONS
{
	. = 0;

    .header : {}

	.text : {
		*(.text*)
	} :text

	.rodata : {
		*(.rodata*)
	} :rodata

	.data : {
		*(.data*)
	} :data

	.dynamic : {
		*(.dynamic)
	} :data :dynamic

	.bss : {
		*(COMMON)
		*(.bss*)
	} :data
}
