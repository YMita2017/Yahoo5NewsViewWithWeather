## ライブラリの修正
## （注意：本修正については、mgo-tecさんやsquix78さんへの問い合わせはしないでください。）

元々のライブラリに対して追加や修正をしています。

基本的なライブラリのインストールは[元記事](https://www.mgo-tec.com/blog-entry-4news-oled-scroll-esp32-dualcore-multitask.html)に従ってください。

基本的に以下のライブラリで動作確認や追加・上書き修正しています。

[ESP32_SD_ShinonomeFNT](https://github.com/mgo-tec/ESP32_SD_ShinonomeFNT)  ( beta ver 1.21 )

[ESP32_SD_UTF8toSJIS](https://github.com/mgo-tec/ESP32_SD_UTF8toSJIS)  ( beta ver 1.21 )

[ESP32_WebGet](https://github.com/mgo-tec/ESP32_WebGet) ( beta ver 1.12 )

Time ライブラリ ( Arduino 標準ライブラリ ver 1.5 )

[MiniGrafx](https://github.com/squix78/minigrafx) (release 0.0.14)



#### ライブラリの追加：

Arduino\Librariesに[ESP32_SD_ILI9341_Gadgets-master](https://github.com/YMita2017/Yahoo5NewsViewWithWeather/tree/master/Libraries/ESP32_SD_ILI9341_Gadgets-master)を追加してください。



#### ライブラリの修正：

##### MiniGrafxの修正

まずは[MiniGrafx](https://github.com/squix78/minigrafx)をライブラリに追加してください。

動作確認はRelese 0.0.14に対して行っています。

次に、[Libraries\Mini_Grafx\src](https://github.com/YMita2017/Yahoo5NewsViewWithWeather/tree/master/Libraries/Mini_Grafx/src) から、7つのファイルを上書きしてMiniGrafxのsrcディレクトリに

入れてください。


#### ESP32_WebGet-masterの修正

[ESP32_WebGet](https://github.com/mgo-tec/ESP32_WebGet)をライブラリに追加してください。

[Libraries\ESP32_WebGet-master\src](https://github.com/YMita2017/Yahoo5NewsViewWithWeather/tree/master/Libraries/ESP32_WebGet-master/src)から、2つのファイルをESP32_WebGet-masterのsrcディレクトリに上書きしてください。
