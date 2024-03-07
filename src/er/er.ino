#include "er_lcd.h"
#include <WiFi.h>


int lcd_buf[256 * 160/4];


const char* ntpServer = "pool.ntp.org";    //"ntp1.aliyun.com";   
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;

const char *ssid = "EASTRISING-2"; //"your ssid";
const char *password = "er33503873";   //"your password";

struct tm timeinfo;

void setup() {

  Serial.begin(115200);
  er_lcd_begin();
  Serial.print("LCD Example\n");
 
}

void loop() {

  /*set_display_rotate(ROTATE_0);
  er_lcd_clear(lcd_buf);
  er_lcd_string(0, 0, "Hollow!", 12, 1, lcd_buf); 
  er_lcd_string(0, HEIGHT/2,  "buydisplay.com", 12, 1, lcd_buf);   
  er_lcd_string(0, HEIGHT-12, "EastRising", 12, 1, lcd_buf);     
  er_lcd_display(lcd_buf);
  delay(1000); 
 
  
 

  set_display_rotate(ROTATE_0);
  er_lcd_clear(lcd_buf);
  er_lcd_bitmap(0, 0, PIC1, 192, 96, lcd_buf);
  er_lcd_display(lcd_buf);
  delay(3000);  

  er_lcd_clear(lcd_buf);
  er_lcd_bitmap(0, 0, PIC2, 192, 96, lcd_buf);
  er_lcd_display(lcd_buf);
  delay(3000);*/
 
  er_lcd_clear(lcd_buf);
  er_lcd_bitmap_gray(0, 0, PIC3, 192, 96, lcd_buf);
  er_lcd_display_gray(lcd_buf);  
  delay(3000);

  er_lcd_clear(lcd_buf);
  er_lcd_bitmap_gray(0, 0, agent2, 48, 48, lcd_buf);
  er_lcd_display_gray(lcd_buf);  
  delay(10000);

  er_lcd_clear(lcd_buf);
  er_lcd_bitmap_gray(0, 0, agent1, 48, 48, lcd_buf);
  er_lcd_display_gray(lcd_buf);  
  delay(10000);
  
  er_lcd_clear(lcd_buf);
  er_lcd_bitmap(0, 0, agent_ii, 48, 48, lcd_buf);
  er_lcd_display(lcd_buf);  
  delay(10000);

  set_display_rotate(ROTATE_0);
 
  er_lcd_clear(lcd_buf);
  demo_sine(lcd_buf);
   
  er_lcd_clear(lcd_buf);
  testLines(lcd_buf);
 
  /*er_lcd_clear(lcd_buf);
  testFastLines(lcd_buf);
 
  er_lcd_clear(lcd_buf);
  testRects(lcd_buf);

  er_lcd_clear(lcd_buf);
  testFilledRects(lcd_buf);

  er_lcd_clear(lcd_buf);
  testFilledCircles(10, lcd_buf);

  er_lcd_clear(lcd_buf);
  testCircles(10,lcd_buf);

  er_lcd_clear(lcd_buf); 
  testTriangles(lcd_buf);

  er_lcd_clear(lcd_buf);
  testFilledTriangles(lcd_buf);

  er_lcd_clear(lcd_buf);
  testRoundRects(lcd_buf); 

  er_lcd_clear(lcd_buf);
  testFilledRoundRects(lcd_buf); 

  er_lcd_clear(lcd_buf);
  demo_game(lcd_buf);


/*
  set_display_rotate(ROTATE_90);
 
  er_lcd_clear(lcd_buf);
  demo_sine(lcd_buf);

 
  er_lcd_clear(lcd_buf);
  testLines(lcd_buf);
 
  er_lcd_clear(lcd_buf);
  testFastLines(lcd_buf);
 
  er_lcd_clear(lcd_buf);
  testRects(lcd_buf);

  er_lcd_clear(lcd_buf);
  testFilledRects(lcd_buf);

  er_lcd_clear(lcd_buf);
  testFilledCircles(10, lcd_buf);

  er_lcd_clear(lcd_buf);
  testCircles(10,lcd_buf);

  er_lcd_clear(lcd_buf); 
  testTriangles(lcd_buf);

  er_lcd_clear(lcd_buf);
  testFilledTriangles(lcd_buf);

  er_lcd_clear(lcd_buf);
  testRoundRects(lcd_buf); 

  er_lcd_clear(lcd_buf);
  testFilledRoundRects(lcd_buf); 


/*
  Serial.printf("Connecting to %s ", ssid);
  set_display_rotate(ROTATE_0);
  er_lcd_clear(lcd_buf);
  er_lcd_string(0, 2, "Connecting to wifi", 12, 1, lcd_buf); 
  er_lcd_display(lcd_buf);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");
      er_lcd_string(0, 16, ".....", 12, 1, lcd_buf); 
      er_lcd_display(lcd_buf);
  }
      Serial.println("WiFi connected");
      Serial.println(WiFi.localIP());
      er_lcd_string(0, 30, "WiFi connected", 12, 1, lcd_buf); 
      er_lcd_display(lcd_buf);




  er_lcd_clear(lcd_buf);
  // display images of bitmap matrix 
  er_lcd_bitmap(0, 0, Signal816, 16, 8, lcd_buf); 
  er_lcd_bitmap(40, 0,Bluetooth88, 8, 8, lcd_buf); 
  er_lcd_bitmap(72, 0, Msg816, 16, 8, lcd_buf); 
  er_lcd_bitmap(112, 0, GPRS88, 8, 8, lcd_buf); 
  er_lcd_bitmap(144, 0, Alarm88, 8, 8, lcd_buf); 
  er_lcd_bitmap(176, 0, Bat816, 16, 8, lcd_buf); 

  

  er_lcd_string(35, 84, "Wifi:EASTRISING-2", 12, 1, lcd_buf); 
 

const String WDAY_NAMES[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturd"};                
const String MONTH_NAMES[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; 
  char value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}; 
   while(1)
   {
  
  
  configTime(gmtOffset_sec, daylightOffset_sec,ntpServer);   
  while(!getLocalTime(&timeinfo));
  if(!getLocalTime(&timeinfo)){Serial.println("Failed to obtain time");}
  Serial.println(&timeinfo, "%a, %Y-%m-%d %H:%M:%S");  
    String date = WDAY_NAMES[timeinfo.tm_wday]; 
    timeinfo.tm_year += 1900 ; 
    timeinfo.tm_mon+=1; 
     
    er_lcd_char3216(32, 20,  value[timeinfo.tm_hour / 10], lcd_buf);
    er_lcd_char3216(48, 20, value[timeinfo.tm_hour % 10], lcd_buf);
    er_lcd_char3216(64, 20, ':', lcd_buf);
    er_lcd_char3216(80, 20, value[timeinfo.tm_min / 10], lcd_buf);
    er_lcd_char3216(96, 20, value[timeinfo.tm_min % 10], lcd_buf);
    er_lcd_char3216(112, 20, ':', lcd_buf);
    er_lcd_char3216(128, 20, value[timeinfo.tm_sec / 10], lcd_buf);
    er_lcd_char3216(144, 20, value[timeinfo.tm_sec % 10], lcd_buf);

  
    er_lcd_char(32, 64,  value[timeinfo.tm_year / 1000],12, 1,lcd_buf);
    er_lcd_char(38, 64,  value[timeinfo.tm_year % 1000/100],12, 1,lcd_buf);
    er_lcd_char(44, 64,  value[timeinfo.tm_year %1000%100/10],12, 1,lcd_buf);
    er_lcd_char(50, 64,  value[timeinfo.tm_year %1000%100%10],12, 1,lcd_buf);  
    er_lcd_char(56, 64, '-',12, 1,lcd_buf);
    er_lcd_char(62, 64,  value[timeinfo.tm_mon / 10],12, 1,lcd_buf);
    er_lcd_char(68, 64,  value[timeinfo.tm_mon %  10],12, 1,lcd_buf);  
    er_lcd_char(74, 64, '-',12, 1,lcd_buf);
    er_lcd_char(80, 64,  value[timeinfo.tm_mday / 10],12, 1,lcd_buf);
    er_lcd_char(86, 64,  value[timeinfo.tm_mday %  10],12, 1,lcd_buf); 

    er_lcd_string(132, 64, WDAY_NAMES[timeinfo.tm_wday].c_str(), 12, 0, lcd_buf);   
    
    er_lcd_display(lcd_buf);
         delay(1000);//Analog clock 1s   
  
    }
   */
}
