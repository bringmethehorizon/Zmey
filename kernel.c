#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}
 
uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}
 
size_t strlen(const char* str)
{
	size_t ret = 0;
	while (str[ret]!=0)
		ret++;
	return ret;
}
char* memcpy(char* dest, const char* src, unsigned int count)
{
	for(size_t i=0; i<count; i++) dest[i]=src[i];
	return dest;
}
char* memset(char* dest, char val, unsigned int count)
{
	for(size_t i=0; i<count; i++) dest[i]=val;
	return dest;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_scroll()
{
	for(size_t i=0; i<VGA_WIDTH; i++) terminal_putentryat(' ', terminal_color, i, 0);
	for(size_t i=1; i<VGA_HEIGHT-1; i++)
	{
		for(size_t j=0; j<VGA_WIDTH; j++)
		{
			size_t index1 = i*VGA_WIDTH+j,index2 = (i+1)*VGA_WIDTH+j;
			terminal_buffer[index1]=terminal_buffer[index2];
		}
	}
	for(size_t i=0; i<VGA_WIDTH; i++)
	{
		terminal_putentryat(' ', terminal_color, i, VGA_HEIGHT-1);
	}
}
 
void terminal_putchar(char c)
{
	if(c=='\n')
	{
		terminal_column=0;
		terminal_row++;
		if(terminal_row==VGA_HEIGHT)
		{
			terminal_scroll();
			terminal_column=0;
			terminal_row--;
		}
		return;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	terminal_column++;
	if(terminal_column==VGA_WIDTH)
	{
		terminal_row++;
		if(terminal_row==VGA_HEIGHT)
		{
			terminal_scroll();
			terminal_column=0;
			terminal_row--;
		}
	}
}
char* itoa(int i, char buffer[])
{
	char const digit[]="0123456789";
	char* p=buffer;
	if(i<0)
	{
		*p++='-';
		i*=-1;
	}
	int shifter=i;
	do
	{
		++p;
		shifter=shifter/10;
	}while(shifter);
	*p='\0';
	do
	{
		*--p=digit[i%10];
		i=i/10;
	}while(i);
	return buffer;
}

void terminal_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; i++)
	{
		terminal_putchar(data[i]);
	}
}

void kernel_main()
{
	char fun[]="  ";
	terminal_initialize();
	terminal_writestring("Zmey v0.0001+epsilon\n"); 
	for(size_t i=0; i<30; i++) 
	{
		terminal_writestring(itoa(i,fun));
		terminal_writestring("\n");
	}
}
