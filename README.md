# Yahoo5NewsViewWithWeather
mgo-tec電子工作さんオリジナルのESP32使用してYahooニュースと天気予報等表示をLCDに移植してみた

mgo-tec電子工作さんが、ESP32+OLEDを使用してYahooニュースや時計、天気予報を表示するアプリを公開されています。
https://www.mgo-tec.com/
OLEDディスプレイは非常に発色が良く、使われているチップSSD1331も性能が良いため、良い動きをします。
しかし、解像度が96x64と狭いのが残念。もう少し色々表示したい。
って事で、各所で売られている2.8インチTFT-LCD(320x240 ILI9341)に移植してみました。
https://www.amazon.co.jp/HiLetgo-LCD%E3%83%87%E3%82%A3%E3%82%B9%E3%83%97%E3%83%AC%E3%82%A4-%E3%82%BF%E3%83%83%E3%83%81%E3%83%91%E3%83%8D%E3%83%AB-SPI%E3%82%B7%E3%83%AA%E3%82%A2%E3%83%AB240-ILI9341/dp/B072N551V3/ref=pd_cp_147_2?_encoding=UTF8&psc=1&refRID=9329M39ZZB2KK5D7VNP4  
https://www.ebay.com/i/201950756171?rt=nc  
https://www.aliexpress.com/item/Consumer-Electronics-Shop-Free-shipping-240x320-2-8-SPI-TFT-LCD-Touch-Panel-Serial-Port-Module/32601714462.html
当然、microSDインターフェイスは付属のSDカードインターフェイスに修正。他にもハードウェアをいじったので、
回路図も参照してください。

しかし、以下の箇所で苦労しました。
1.SSD1331でサポートされている、CopyコマンドがILI9341に無い。
2.そのまま移植すると、表示領域が広すぎて画面がもったいない。
3.連続動作していると、何故か文字が意味不明のドットに化ける

1.CopyはSPIインターフェイスでLCDから描画を読みだして、ずらして書き込みすると、遅すぎて使えないので、
　仮想VRAM的動作のMiniGrafxライブラリ https://github.com/squix78/minigrafx を使って、ESP32側で
  スクロールし、仮想VRAMから転送することで実現。
  元々、仮想VRAMからの転送方式はRetroPieでそこそこ早い事が判っていたので、方式採用。
  https://www.youtube.com/watch?v=nE0DtXGXHAw
  http://www.sudomod.com/forum/viewtopic.php?t=2312
2.そのまま移植したら、画面の1/54程度に全て収まったので、4Yahooニュースに1行追加、更に時計・天気予報を追加
  して、やっと1/4程度に。
  しかし、そのままではせっかくの画面がもったいないので、縦横2倍に拡大して表示する事にした。
  当初、フォントから描画時に縦横2倍にしたが、仮想VRAMがメモリを圧迫するので、仮想VRAMを1/4サイズとして、
  転送時に縦横2倍にすることで、メモリ領域も描画速度もアップ。
3.本来、あってはいけないことだが、何故かSDカードのフォント読み出しファイルポインタが破壊される。
  ハードウェアの品質？ライブラリの問題？結局解決できずに、ニュース読み込みタイミングで、フォントファイルを
  クローズ・オープンする事で不具合に蓋をした．．．。(1週間動作したので、あきらめた．．．)
