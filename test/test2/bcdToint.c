void LCD_DisplayRtcTime(char hour,char min,char sec)
{
    LCD_DataWrite(((hour>>4) & 0x0f) + 0x30);
    LCD_DataWrite((hour & 0x0f) + 0x30);
    LCD_DataWrite(':');
 
    LCD_DataWrite(((min>>4) & 0x0f) + 0x30);
    LCD_DataWrite((min & 0x0f) + 0x30);
    LCD_DataWrite(':');
 
    LCD_DataWrite(((sec>>4) & 0x0f) + 0x30);
    LCD_DataWrite((sec & 0x0f) + 0x30);
 
 }