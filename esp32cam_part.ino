#include "time.h"
#include <WiFi.h>
#include "esp_camera.h"
#include "esp_system.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 0;
bool Flash_on= true;

hw_timer_t *timer = NULL;
void IRAM_ATTR resetModule(){
    ets_printf("reboot\n");
    esp_restart();
}
#include <TridentTD_LineNotify.h>
#define SSID        "true_home2G_70f"   //WiFi name
#define PASSWORD    "81204900"   //PASSWORD
#define LINE_TOKEN  "ELvNEdEyx7wnxWxVLHBRnqvPvwwSEUehjTIHE440tcL"   

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
char DateTime[20];
const int Led_Flash = 4;
const int trigPin = 12;
const int echoPin = 13;
boolean startTimer = false;
int chkLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return 0;
  }
  char Hour[3];
  char Min[3];
  strftime(DateTime,20,"%x %X",&timeinfo);
  strftime(Hour,3, "%H", &timeinfo);
  //strftime(Min,3, "%M", &timeinfo);
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  int hour = atoi(Hour);
  //int minute = atoi(Min);
//  Serial.print(hour);
//  Serial.println(minute);
  if((hour>=18 || hour<6)){
    return 1;
  }
  return 0;
}
 void Camera_capture(String msg) {
  if( chkLocalTime()){
    digitalWrite(Led_Flash, HIGH);
    delay(100); 
    digitalWrite(Led_Flash, LOW);
    delay(100);
    digitalWrite(Led_Flash, HIGH); 
  }
  camera_fb_t * fb = NULL;
  delay(100); 
  // Take Picture with Camera
  fb = esp_camera_fb_get(); 
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
   digitalWrite(Led_Flash, LOW);
   Send_line(fb->buf,fb->len,msg);
   esp_camera_fb_return(fb); 
  // Serial.println("Going to sleep now");
  // esp_deep_sleep_start();
  // Serial.println("This will never be printed");

}

void Send_line(uint8_t *image_data,size_t   image_size,String msg){
   String messsege = msg +" at: \n\n"+ DateTime;
   LINE.notifyPicture(messsege,image_data, image_size);
  }
void setup() {

  Serial.begin(115200);
   while (!Serial) {  ;  }
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  pinMode(Led_Flash, OUTPUT);
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) {
    while (!time(nullptr)) {
      Serial.print("*");delay(1000);
    } 
    }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  chkLocalTime();
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  
  LINE.setToken(LINE_TOKEN);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
// FRAMESIZE_ +
//QQVGA/160x120//QQVGA2/128x160//QCIF/176x144//HQVGA/240x176
//QVGA/320x240//CIF/400x296//VGA/640x480//SVGA/800x600//XGA/1024x768
//SXGA/1280x1024//UXGA/1600x1200//QXGA/2048*1536
    config.frame_size = FRAMESIZE_SXGA; 
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

char test = ' '; 

void loop() {
  while (Serial.available()) {
    test = Serial.read();
    if(test == 'c')
    {
      Camera_capture("Mail has arrived");
    }else if(test == 'a'){
      Camera_capture("Someone try to open mail box");
    }
  }
}
