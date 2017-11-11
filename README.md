# Yahoo5NewsViewWithWeather
## 本プログラムに対する問い合わせはmgo-tec電子工作さんやsquix78さんへ問い合わせはしないでください。）
mgo-tec電子工作さんオリジナルのESP32を使用したYahooニュースと天気予報等表示をLCDに移植してみました

[mgo-tec電子工作さん](https://www.mgo-tec.com/)が、ESP32+OLEDを使用してYahooニュースや時計、天気予報を表示するアプリを公開されています。


OLEDディスプレイは非常に発色が良く、使われているチップSSD1331も性能が良いため、良い動きをします。

しかし、解像度が96x64と狭いのが残念。もう少し色々表示したい。
って事で、各所で売られている2.8インチTFT-LCD(320x240 ILI9341)に移植してみました。

[アマゾンの2.8インチTFT-LCD(320x240 ILI9341)](https://www.amazon.co.jp/HiLetgo-LCD%E3%83%87%E3%82%A3%E3%82%B9%E3%83%97%E3%83%AC%E3%82%A4-%E3%82%BF%E3%83%83%E3%83%81%E3%83%91%E3%83%8D%E3%83%AB-SPI%E3%82%B7%E3%83%AA%E3%82%A2%E3%83%AB240-ILI9341/dp/B072N551V3/ref=pd_cp_147_2?_encoding=UTF8&psc=1&refRID=9329M39ZZB2KK5D7VNP4)

[ebayの2.8インチTFT-LCD(320x240 ILI9341)](https://www.ebay.com/i/201950756171?rt=nc)

[aliexpressの2.8インチTFT-LCD(320x240 ILI9341)多数](https://www.aliexpress.com/item/Consumer-Electronics-Shop-Free-shipping-240x320-2-8-SPI-TFT-LCD-Touch-Panel-Serial-Port-Module/32601714462.html)


当然、microSDインターフェイスは付属のSDカードインターフェイスに修正。

他にもハードウェアをいじったので、回路図も参照してください。

[ハードウェア](https://github.com/YMita2017/Yahoo5NewsViewWithWeather/tree/master/Hardware)

ソフトウェアは本ディレクトリのYahoo5NewsViewWithWeather.inoの他に、Librarisのライブラリ追加や修正が必要です。

[ライブラリ](https://github.com/YMita2017/Yahoo5NewsViewWithWeather/tree/master/Libraries)

また、動作にはフォントや元記事にある、各種ファイルをSDカードに入れる必要があります。（著作権等、元記事をしっかり読んで理解してください。）



今回移植で、以下の箇所で苦労しました。

#### 1.SSD1331でサポートされている、CopyコマンドがILI9341に無い。

#### 2.そのまま移植すると、表示領域が広すぎて画面がもったいない。

#### 3.メモリ領域が足りない。

#### 4.連続動作していると、何故か文字が意味不明のドットに化ける

以下はどう解決したか．．．


#### 1.SSD1331でサポートされている、CopyコマンドがILI9341に無い。

  CopyはSPIインターフェイスでLCDから描画を読みだして、ずらして書き込みすると、遅すぎて使えないので、

　仮想VRAM的動作のMiniGrafxライブラリ https://github.com/squix78/minigrafx を使って、ESP32側で
 
  スクロールし、仮想VRAMから転送することで実現。
  
  元々、仮想VRAMからの転送方式はRetroPieでそこそこ早い事が判っていたので、方式採用。
  
  [RetropieでILI9341使用動画](https://www.youtube.com/watch?v=nE0DtXGXHAw)
  
  [RetropieのILI9341使用例](http://www.sudomod.com/forum/viewtopic.php?t=2312)


#### 2.そのまま移植すると、表示領域が広すぎて画面がもったいない。

  そのまま移植したら、画面の1/5程度に全て収まったので、4Yahooニュースに1行追加、更に時計・天気予報を追加

  して、やっと1/4程度に。
  
  しかし、そのままではせっかくの画面がもったいないので、縦横2倍に拡大して表示する事にした。
  
  当初、フォントから仮想VRAM描画時に縦横2倍にしたが、仮想VRAMがメモリを圧迫するので、仮想VRAMを1/4サイズとして、
  
  転送時に縦横2倍にすることで、メモリ領域も描画速度もアップ。


#### 3.メモリ領域が足りない。

  仮想VRAMやYahooニュース5本を表示しようとメモリを使いすぎてメモリ不足！でも、以下の記事を読んで禁断の
  
  メモリ開放を使っています。
  
  [Need More SRAM](https://www.esp32.com/viewtopic.php?t=1482)
  
  簡単に言えば、使っていないBluetoothを使わない設定にし、メモリを解放。
  
  ESP32を使用するのに、ハードウェア追加していますが、インストールした以下のディレクトリの
  
  ファイルを編集。（バックアップしてから編集してくださいね！）
  
  Arduino\hardware\espressif\esp32\tools\sdk\include\config\sdkconfig.h
  
  以下の様に修正。
  #define CONFIG_BT_RESERVE_DRAM 0x10000  ->  #define CONFIG_BT_RESERVE_DRAM 0x0
  
  #define CONFIG_BTC_TASK_STACK_SIZE 8192  ->  #define CONFIG_BTC_TASK_STACK_SIZE 1024
  
  #define CONFIG_BT_ENABLED 1  ->  #define CONFIG_BT_ENABLED 0
  
  これでかなりのメモリが解放されます。


#### 4.連続動作していると、何故か文字が意味不明のドットに化ける

  本来、あってはいけないことだが、何故かSDカードのフォント読み出しファイルポインタが破壊される。

  ハードウェアの品質？ライブラリの問題？結局解決できずに、ニュース読み込みタイミングで、フォントファイルを
  
  クローズ・オープンする事で不具合に蓋をした．．．。(1週間連続動作したので、追及をあきらめた．．．)
