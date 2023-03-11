#include "lcd1602.h"

static void LCD_Cmd(uint8_t cmd);
static void LCD_Data(uint8_t data);
static void LCD_CursorSet(uint8_t col, uint8_t row);
static void LCD_EnBlink(void);

static LCD_Options_t LCD_Opts;

void LCD_Delay_us(uint16_t us)
{
	uint32_t  Div = (SysTick->LOAD+1)/1000;
  uint32_t  StartMicros = HAL_GetTick()*1000 + (1000- SysTick->VAL/Div);
  while((HAL_GetTick()*1000 + (1000-SysTick->VAL/Div)-StartMicros < us));
}

void LCD_Delay_ms(uint8_t ms)
{
	HAL_Delay(ms);
}

void LCD_Init(void)
{
	LCD_Opts.currentX = 0;
  LCD_Opts.currentY = 0;
  LCD_Opts.DisplayFunction = LCD_8BITMODE | LCD_5x8DOTS | LCD_1LINE;
  if (_LCD_ROWS > 1)
	{
		LCD_Opts.DisplayFunction |= LCD_2LINE;
	}
  LCD_Cmd(LCD_FUNCTIONSET | LCD_Opts.DisplayFunction);
  LCD_Opts.DisplayControl = LCD_DISPLAYON;
  LCD_DisplayOn();
  LCD_Opts.DisplayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  LCD_Cmd(LCD_ENTRYMODESET | LCD_Opts.DisplayMode);
  LCD_Clear();
  LCD_Delay_ms(5);
}

void LCD_Clear(void)
{
  LCD_Cmd(LCD_CLEARDISPLAY);
  LCD_Delay_ms(5);
}

void LCD_Puts(uint8_t x, uint8_t y, char* str)
{
  LCD_CursorSet(x, y);
  while(*str)
	{
		if(LCD_Opts.currentX >= _LCD_COLS)
		{
			LCD_Opts.currentX = 0;
      LCD_Opts.currentY++;
      LCD_CursorSet(LCD_Opts.currentX, LCD_Opts.currentY);
		}
    if (*str == '\n')
		{
			LCD_Opts.currentY++;
			LCD_CursorSet(LCD_Opts.currentX, LCD_Opts.currentY);
    }
		else if(*str == '\r')
		{
			LCD_CursorSet(0, LCD_Opts.currentY);
    }
		else
		{
			LCD_Data(*str);
      LCD_Opts.currentX++;
    }
    str++;
  }
}

void LCD_DisplayOn(void)
{
	LCD_Opts.DisplayControl |= LCD_DISPLAYON;
  LCD_Cmd(LCD_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_DisplayOff(void)
{
	LCD_Opts.DisplayControl &= ~LCD_DISPLAYON;
  LCD_Cmd(LCD_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_BlinkOn(void)
{
	LCD_Opts.DisplayControl |= LCD_BLINKON;
  LCD_Cmd(LCD_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_BlinkOff(void)
{
	LCD_Opts.DisplayControl &= ~LCD_BLINKON;
  LCD_Cmd(LCD_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_CursorOn(void)
{
  LCD_Opts.DisplayControl |= LCD_CURSORON;
  LCD_Cmd(LCD_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_CursorOff(void)
{
  LCD_Opts.DisplayControl &= ~LCD_CURSORON;
  LCD_Cmd(LCD_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_ScrollLeft(void)
{
  LCD_Cmd(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCD_ScrollRight(void)
{
  LCD_Cmd(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCD_CreateChar(uint8_t location, uint8_t *data)
{
	uint8_t i;
  location &= 0x07;
  LCD_Cmd(LCD_SETCGRAMADDR | (location << 3));
	for (i = 0; i < 8; i++) 
	{
		LCD_Data(data[i]);
  }
}

void LCD_PutCustom(uint8_t x, uint8_t y, uint8_t location)
{
	LCD_CursorSet(x, y);
  LCD_Data(location);
}

static void LCD_EnBlink(void)
{
	LCD1602_E_HIGH;
  LCD_Delay_us(50);
  LCD1602_E_LOW;
  LCD_Delay_us(50);
}

static void LCD_Cmd(uint8_t cmd)
{
	LCD1602_RS_CMD;
  LCD1602_RW_WRITE;
  LCD1602_DATA(cmd);
  LCD_EnBlink();
}

static void LCD_Data(uint8_t data)
{
	LCD1602_RS_DATA;
  LCD1602_RW_WRITE;
  LCD1602_DATA(data);
  LCD_EnBlink();
}

static void LCD_CursorSet(uint8_t col, uint8_t row)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  if(row >= _LCD_ROWS)
	{
		row = 0;
	}
  LCD_Opts.currentX = col;
  LCD_Opts.currentY = row;
  LCD_Cmd(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_Put(uint8_t Data)
{
	LCD_Data(Data);
}
