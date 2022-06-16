# GP2040 Firmware 初音ミク Project DIVA 拡張版

GP2040は、Raspberry Pi PicoおよびRP2040マイコンベースのその他のボード用ゲームパッドファームウェアであり、複数のプラットフォームにまたがる豊富な機能セットを備えた高性能を提供します。詳細は、[オリジナル](https://github.com/FeralAI/GP2040)のドキュメントをご覧ください。

このリポジトリは、GP2040を拡張し、タッチセンサモジュールを接続できるようにしたものです。現状、スライドの同時押しは未対応です。

## 対応タッチセンサ

* [Adafruit MPR121 12-Key Capacitive Touch Sensor](https://www.adafruit.com/product/1982) またはMPR121チップ使用同等品  
日本国内では、秋月電子通商・スイッチサイエンス・マルツ 等で購入できます。AliExpressでも安価で同等のものが購入できますが、Adafruit製のものをおすすめします。このファームウェアは、Adafruit製のライブラリを改変したものが含まれています。このライブラリが無ければ完成しませんでした。ありがとうAdafruit。

## 遅延

[自作の遅延測定アプリ](https://github.com/shigobu/gamepadLatencyCheckerShigobu )を使用し、測定しました。オリジナルGP2040より 1 ms 程度長くなっています。 

| Min | Max | Avg |
| - | - | - |
| 0.80 ms | 2.28 ms | 1.56 ms |

## インストール方法

オリジナルとほとんど変わらないので、オリジナルを翻訳したものを載せます。

ビルド済み `uf2` ファイルは [Releases](https://github.com/shigobu/GP2040_With_HatsuneMikuPjdTouchSlider/releases) セクションで入手できます。配線図は[こちら](https://raw.githubusercontent.com/shigobu/GP2040_With_HatsuneMikuPjdTouchSlider/main/configs/Pico/assets/PinMapping.png)。

手順は、デバイスによって若干異なります。 これらの手順は、RaspberryPiPico用です。

> デバイスが以前にGP2040以外で使用されたことがある場合は、最初にこのファイルをフラッシュして、オンボードストレージをクリアしてください：[flash_nuke.uf2](docs/downloads/flash_nuke.uf2)。 nukeファイルをフラッシュした後、clearプログラムが実行され、RPI-RP2ドライブが再表示されるまで1分待ちます。

1. [Releases](https://github.com/shigobu/GP2040_With_HatsuneMikuPjdTouchSlider/releases)セクションから最新の `GP2040-RaspberryPiPico-39pjd_vX.X.Xmiku.uf2` ファイルをダウンロードします。
1. Picoのプラグを抜きます。
1. PicoのBOOTSELボタンを押したまま、コンピューターに接続します。 `RPI-RP2`という名前の新しいリムーバブルドライブがファイルエクスプローラーに表示されます。
1. `GP2040-RaspberryPiPico-39pjd_vX.X.Xmiku.uf2`ファイルをリムーバブルドライブにドラッグアンドドロップします。 これにより、ボードがフラッシュされます。
1. ボードはGP2040ファームウェアを実行しており、コンピューターにコントローラーとして表示されます。

## 課題
* スライドの同時押に対応する。
* 遅延をオリジナルGP2040と同程度にする。

## サポート

タッチスライダーに関する機能、問題、またはその他のことについて話し合いたい場合は、[issueを作成](https://github.com/shigobu/GP2040_With_HatsuneMikuPjdTouchSlider/issues/new)してください。

その他の機能に関することは、オリジナルのドキュメントを参照してください。

## Acknowledgements

* Ha Thach's excellent [TinyUSB library](https://github.com/hathach/tinyusb) examples
* fluffymadness's [tinyusb-xinput](https://github.com/fluffymadness/tinyusb-xinput) sample
* Kevin Boone's [blog post on using RP2040 flash memory as emulated EEPROM](https://kevinboone.me/picoflash.html)
* [bitbank2](https://github.com/bitbank2) for the [OneBitDisplay](https://github.com/bitbank2/OneBitDisplay) and [BitBang_I2C](https://github.com/bitbank2/BitBang_I2C) libraries, which were ported for use with the Pico SDK
* [Adafruit MPR121 Library](https://github.com/adafruit/Adafruit_MPR121)