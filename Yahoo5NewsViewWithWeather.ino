#include "ESP32_SD_ILI9341_Gadgets.h" // modify
#include "ESP32_SD_ShinonomeFNT.h" //beta ver 1.21
#include "ESP32_SD_UTF8toSJIS.h" //beta ver 1.21
#include <MiniGrafx.h>  // add
#include <MiniGrafxAdd.h> // add
#include <ILI9341LR_SPI.h>// add
 
const char* ssid = "yourssid"; //ご自分のルーターのSSIDに書き換えてください。
const char* password = "yourpassword"; //ご自分のルーターのパスワードに書き換えてください。
 
const char* UTF8SJIS_file = "/font/Utf8Sjis.tbl"; //UTF8 Shift_JIS 変換テーブルファイル名を記載しておく
const char* Shino_Zen_Font_file = "/font/shnmk16.bdf"; //全角フォントファイル名を定義
const char* Shino_Half_Font_file = "/font/shnm8x16.bdf"; //半角フォントファイル名を定義
const char* MyFont_file = "/font/MyFont.fnt"; //自作フォントファイル名を定義
const char* Yahoo_rootca_file = "/root_ca/yahoo.cer"; //Yahoo! Japan RSS ルート証明書ファイル
 
const uint8_t CS_SD = 5; //SD card CS ( Chip Select )
 
//const uint8_t SCLK_OLED =  14; //SCLK                               // use same pin with microSD
//const uint8_t MOSI_OLED =  13; //MOSI (Master Output Slave Input)   // use same pin with microSD
//const uint8_t MISO_OLED =  12; //これは実際は使っていない。MISO (Master Input Slave Output)   // use same pin with microSD
const uint8_t CS_OLED = 4;
const uint8_t DC_OLED =  16; //OLED DC(Data/Command)
const uint8_t RST_OLED =  15; //OLED Reset
const uint8_t LED_OLED = 2; // LCD back light
 
uint16_t palette[] = {ILI9341_BLACK, // 0 add palette definition for MiniGrafx
                      0x07e0, // 1
                      0x8410, // 2
                      0xffff, // 3
                      0xf804, // 4
                      0xfbe0, // 5
                      0x7bef, // 6
                      0x001f, // 7
                      0xffe0,   // 8
                      ILI9341_GREEN,  // 9
                      ILI9341_ORANGE, // 10
                      0x07e0,  //11
                      0x8410,  // 12
                      0xffff,  // 13
                      0xf804,   // 14
                      ILI9341_GREENYELLOW,  // 15
                      0x7E3C
                      }; //3

boolean FDupdate = false;

//ESP32_SD_SSD1331_Gadgets ESSG;
ESP32_SD_ILI9341_Gadgets ESSG;
ESP32_SD_UTF8toSJIS u8ts;
//ESP32_SSD1331 _ssd1331(SCLK_OLED, MISO_OLED, MOSI_OLED, CS_OLED, DC_OLED, RST_OLED); //HSPI 接続
ILI9341LR_SPI tft = ILI9341LR_SPI(CS_OLED, DC_OLED, RST_OLED);
//uint8_t bufmem[380*240/2];
MiniGrafxAdd _ili9341 = MiniGrafxAdd(&tft, 4, palette);
//MiniGrafxAdd _ili9341 = MiniGrafxAdd(&tft, 4, palette, bufmem);
ESP32_SD_ShinonomeFNT _SFR(CS_SD, 24000000); //SPI 24MHz
ESP32_WebGet _EWG;
 
//Yahoo! Japan RSS トピックス以外のニュースは2048バイト以上の配列確保必要
uint8_t sj_txt[5][2048] = {}; //Shift_JIS コード文字列を格納
//uint8_t sj_txt[1][2048] = {}; //Shift_JIS コード文字列を格納
uint16_t sj_length[5] = {}; //Shift_JIS コード文字列長
 
int32_t NewsGetLastTime = 0; //起動時にすぐGETするようにマイナスにしている
bool WebGet[5] ={false, false, false, false, false}; //記事をGETした場合はtrue
 
//-----NTP時刻GET設定---------
uint32_t NTP_LastTime = 0;
 
//-----Yahoo記事取得引数初期化-------------------------
uint64_t WeatherGetLastTime = 0;
//bool News_first_get = true;
bool Weather_first_get = true;
//bool News_get = false;
bool Weather_get = false;
String weather_str;
 
portTickType xLastWakeWebGetTime1; //マルチタスク時間カウント
 
//***************** Arduino Setup **********************************
void setup() {
  _EWG.EWG_AP_Connect(ssid, password); //Wi-Fi ルーターと接続
  delay(1000);
 
//  _ssd1331.SSD1331_Init(); //OLED SSD1331 初期化
  pinMode(LED_OLED, OUTPUT);    // backlight LED pin set as output
  digitalWrite(LED_OLED, HIGH); // backlight LED on
  _ili9341.init();              // initialize LCD display
  _ili9341.setRotation(3);      // rotate display as width long
  _ili9341.clear();             // clear screen

  //UTF-8→S_JIS変換テーブル、東雲フォント全角、半角ファイル、合計３つのファイルを同時に開く
  if(!_SFR.SD_Shinonome_Init3F(UTF8SJIS_file, Shino_Half_Font_file, Shino_Zen_Font_file)){
    return;
  }
 
   ESSG.Gadgets_MyFont_Init(MyFont_file); //自作フォント初期化
  delay(10);
 
  _EWG.EWG_NTP_TimeLib_init(9, "time.windows.com"); //NTPサーバー取得初期化
  _EWG.NTP_OtherServerSelect(9); //NTPサーバーと接続できなかった場合、他のNTPサーバーと接続できるか試す関数
 
 //EDN Japan 初回GET
  WebGet[0] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/it_edn-c_sci.xml", Yahoo_rootca_file, sj_txt[0], &sj_length[0]);
  //日刊スポーツ 初回GET
  WebGet[1] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/nksports-c_spo.xml", Yahoo_rootca_file, sj_txt[1], &sj_length[1]);
  //東スポWeb 初回GET
  WebGet[2] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/tospoweb-c_ent.xml",  Yahoo_rootca_file, sj_txt[2], &sj_length[2]);
  //日経トレンディネット 初回GET
  WebGet[3] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/trendy-all.xml", Yahoo_rootca_file, sj_txt[3], &sj_length[3]);
  //Yahoonニュースピックアップ 初回GET
  WebGet[4] = Yahoo_RSS_GET("news.yahoo.co.jp", "/pickup/rss.xml", Yahoo_rootca_file, sj_txt[4], &sj_length[4]);
  NewsGetLastTime = millis();
 
  TaskHandle_t th; //ESP32 マルチタスク　ハンドル定義
  xTaskCreatePinnedToCore(Task1, "Task1", 4096, NULL, 5, &th, 0); //マルチタスク core 0 実行
}
//***************** メインループ **********************************
void loop() {
//void Task1(void *pvParameters){
  if((millis() - NewsGetLastTime) > 180000){ //180秒毎に記事更新
    FDupdate = true;
    //EDN Japan
    WebGet[0] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/it_edn-c_sci.xml", Yahoo_rootca_file, sj_txt[0], &sj_length[0]);
    //日刊スポーツ
    WebGet[1] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/nksports-c_spo.xml", Yahoo_rootca_file, sj_txt[1], &sj_length[1]);
    //東スポWeb
    WebGet[2] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/tospoweb-c_ent.xml", Yahoo_rootca_file, sj_txt[2], &sj_length[2]);
    //日経トレンディネット
    WebGet[3] = Yahoo_RSS_GET("headlines.yahoo.co.jp", "/rss/trendy-all.xml", Yahoo_rootca_file, sj_txt[3], &sj_length[3]);
    //Yahoonニュースピックアップ
    WebGet[4] = Yahoo_RSS_GET("news.yahoo.co.jp", "/pickup/rss.xml", Yahoo_rootca_file, sj_txt[4], &sj_length[4]);
    NewsGetLastTime = millis();
  }
  YahooWeatherGET(24, 180000); //180秒毎に天気取得
  _EWG.NTP_Get_Interval(180000); //180秒毎にNTPサーバーから時刻取得
  vTaskDelay(1);
}
 
//************* マルチタスク ****************************************
void Task1(void *pvParameters){
//void loop(){
  while(1){ //red(0-7), green(0-7), blue(0-3)
    if (FDupdate) {
      FontFileCloseOpen();
      FDupdate = false;
    }
    ESSG.Scroll_1_line(0, 0, 1, 1, sj_txt[0], sj_length[0], &WebGet[0]); // 40
    ESSG.Scroll_1_line(16, 1, 7, 4, sj_txt[1], sj_length[1], &WebGet[1]);  // 80
    ESSG.Scroll_1_line(32, 2, 4, 7, sj_txt[2], sj_length[2], &WebGet[2]); // 120
    ESSG.Scroll_1_line(48, 3, 3, 11, sj_txt[3], sj_length[3], &WebGet[3]); // 160
    ESSG.Scroll_1_line(64, 4, 8, 15, sj_txt[4], sj_length[4], &WebGet[4]); // 160
    ESSG.ShinonomeClock_YMD(0, 82, 2); //東雲フォント年月日表示                                 // color = 0b10010011 = 0x67 = 0b1000010000010000 = 0x8410  2
    ESSG.MyFontClock_Weekday(80, 82, 3); //自作フォント曜日表示                                 // color = 0b11111111 = 0xff = 0b1111111111111111 = 0xffff  3
    ESSG.Shinonome_Sec_Clock(80, 100, 'H', 'V', 0, 0, 15, 4); //東雲フォント秒表示               // color = 0b11100001 = 0xe1 = 0b1111100000000100 = 0xf804  4
    ESSG.MyFont_HM_Clock(0, 100, 'H', 'V', 'H', 'V', 0, 0, 1, 1, 50, 4); //自作フォント時分表示  // color = 0b11100001 = 0xe1 = 0b1111100000000100 = 0xf804  4
 
    if(Weather_get){ //サーバーから天気予報取得できたら、OLED表示を更新
      ESSG.YahooJ_Weather_TodayTomorrow2(82, weather_str);
      Weather_get = false;
    }
  _ili9341.commitdouble();
  vTaskDelay(1);
  }
}
//************* Yahoo! Japan RSS GET ********************************
bool Yahoo_RSS_GET(const char * Host, String t_ip, const char *Rootca_File, uint8_t sj[], uint16_t *sj_len){
  char root_ca[2048];
  ESSG.Root_CA_SDcard_Read(Rootca_File, root_ca);
  String news_str = _EWG.EWG_https_Web_Get(root_ca, Host, t_ip, '\n', "</rss>", "<title>", "</title>", "◆ ");
  *sj_len = u8ts.UTF8_to_SJIS(news_str, sj);
  Serial.println(news_str);
  Serial.flush();
  return true;  
}
//************** Yahoo RSS 天気予報ガジェット *************************
void YahooWeatherGET(uint8_t y0, uint32_t get_interval){
  if((Weather_first_get == true) || ((millis() - WeatherGetLastTime) > get_interval)){
 
    char root_ca[2048];
    ESSG.Root_CA_SDcard_Read(Yahoo_rootca_file, root_ca); //SDカードに保存してあるルート証明書取得
    //東京の天気予報
    weather_str = _EWG.EWG_https_Web_Get(root_ca, "rss-weather.yahoo.co.jp", "/rss/days/46.xml",  '>', "</rss", "】 ", " - ", "|");
    //岐阜の天気予報
//    weather_str = _EWG.EWG_https_Web_Get(root_ca, "rss-weather.yahoo.co.jp", "/rss/days/5210.xml",  '>', "</rss", "】 ", " - ", "|");
    Serial.print("Weather forecast = "); Serial.println(weather_str);
    Serial.flush(); //シリアル出力が終わるまで待つ
    WeatherGetLastTime = millis();
    Weather_get = true;
    Weather_first_get = false;
  }
}

void FontFileCloseOpen() {
  _SFR.SD_Shinonome_Close3F();
  _SFR.SD_Shinonome_Init3F(UTF8SJIS_file, Shino_Half_Font_file, Shino_Zen_Font_file);
  ESSG.Gadgets_MyFont_Close();
  ESSG.Gadgets_MyFont_Init(MyFont_file);
}

