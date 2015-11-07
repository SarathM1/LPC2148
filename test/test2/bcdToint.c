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

 void LCD_DisplayRtcDate(char day,char month,char year)
 {
     LCD_DataWrite(((day>>4) & 0x0f) + 0x30);
     LCD_DataWrite((day & 0x0f) + 0x30);
     LCD_DataWrite('/');
 
     LCD_DataWrite(((month>>4) & 0x0f) + 0x30);
     LCD_DataWrite((month & 0x0f) + 0x30);
     LCD_DataWrite('/');
 
     LCD_DataWrite(((year>>4) & 0x0f) + 0x30);
    LCD_DataWrite((year & 0x0f) + 0x30);
 
 }