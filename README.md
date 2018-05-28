
# Title

- NTigapon

===

## Overview
Arduino sketch and ESP-IDF project.


## Description

sketch名は[マイコンチップ名]-[コントローラ名]-[ターゲット名]とする

- arduino
    - ESP32_ps3_rb0021
        - 2018/7/7-8に開催された[NT金沢](http://wiki.nicotech.jp/nico_tech/index.php?NT%E9%87%91%E6%B2%A22018)に出展した作品のArduino sketchである
        - 簡単な紹介は[NT金沢向け資料]()参照のこと
        - Nexus robot製品の[メカナムホイールロボット](http://www.vstone.co.jp/products/nexusrobot/download/nexus_10021.pdf)を改造する
        - メカナムホイールロボットにESC×1とUSBホストシールドとBLEドングルを追加し、Arduinoで制御する
        - PS3リモコン
            - 
            - 以下のコントロールを使用する
                - 左スティック(移動)
                - 右スティック(ロール)
    - arduino_wii_Roadster
        - 2017/12/16-17に開催された[NT加賀](http://wiki.nicotech.jp/nico_tech/index.php?NT%E5%8A%A0%E8%B3%802017)に出展した作品のArduino sketchである
        - 簡単な紹介は[NT加賀向け資料](https://1drv.ms/p/s!AnC3THzAcjDOgTqdw6XDkZbDYBN4)参照のこと
        - 市販品の1/10スケールラジコンカー[ROADSTER](http://www.tamiya.com/japan/products/57891/index.html)を改造する
        - ROADSTERにサーボ×1とESC×1と超音波距離センサー×2を追加し、Arduinoで制御する
        - Wiiリモコン
            - 接続が切れると緊急停止(Wii.getPitch()とWii.getRoll()が一定回数変化しなければ切断と判断)
            - 以下のボタンを使用する
                - Aボタン(I2Cに'A'を送信)
                - Bボタン(ステアリング切り替え)
                    - UPボタン(ステアリング左)/DOWNボタン(ステアリング右)
                    - Wii.getPitch()の260-160°(ステアリング左)/Wii.getPitch()の160-100°(ステアリング右)
                - -ボタン(ステアリングトリム左)/+ボタン(ステアリングトリム右)
                - 1ボタン(加速)/2ボタン(減速)
                - Homeボタン(切断)
        - 超音波センサーで距離を測って衝突回避(緊急停止)
    - arduino_wii_Hastler
        - 2017/7/8-9に開催された[NT金沢](http://wiki.nicotech.jp/nico_tech/index.php?NT%E9%87%91%E6%B2%A22017)に出展した作品のArduino sketchである
        - 簡単な紹介は[NT金沢向け資料](https://1drv.ms/p/s!AnC3THzAcjDOgTi-qVfNM3mMPAdq)参照のこと
        - 市販品の1/20スケールラジコンカー[Hustler](https://www.amazon.co.jp/dp/B072JSL6Z9/ref=pe_2107282_266464282_TE_3p_dp_1)を改造する
        - Hustlerのモーター×2とLED×2に光センサーを追加し、Arduinoで制御する
        - WiiリモコンのUP(ステアリング左)/DOWN(ステアリング右)/1(加速)/2(減速)ボタンを使用する
        - NT加賀向けにFritzing回路図を用意した
            - arduino_mini_pro.fzz
            - arduino_uno.fzz
        - 回路図の電源は以下を参照のこと
            - arduino Uno(5V)の電源
                - 7-12VならそのままVinにInput、R5は短絡
                - 5.25-7VはVCC3にInputと、DC-DC降圧回路で5Vにしてarduinoの5V PINにInput
                    - 単3電池×4
                - 4.75V以下はDC-DC昇圧回路で5Vにしてarduinoの5V PINにInput、R5は短絡
                    - 単3電池×3
                - USB接続は4.75-5.25Vでarduinoの5V PINにInput相当、R5は短絡
            - arduino pro mini(3.3V)の電源
                - VCC1に電源を接続し、DC-INとDC-OUTを使用して外部にDC-DC回路を接続する
                    - 5.25V以上はDC-DC降圧回路で5VにしてRAWにInput、R5は短絡
                        - 単3電池×4
                    - 4.75V以下はDC-DC昇圧回路で5VにしてRAWにInputとR5の回路側に接続
                        - 単3電池×3
    - ESP32_Blynk_Roadster
        - 市販品の1/10スケールラジコンカー[ROADSTER](http://www.tamiya.com/japan/products/57891/index.html)を改造する
        - ROADSTERにサーボ×1とESC×1を追加し、ESP32で制御する
        - スマホアプリの[Blynk](https://www.blynk.cc/)でESP32をコントロールする
    - モジュール動作確認用各種スケッチ
        - RB0021_Omni4WD_PID
            - 
        - arduino_HS-SR04_Ultrasonic
            - HS-SR04超音波センサ二つ測定
        - arduino_I2C
            - arduino_I2C_MasterWriter
            - arduino_I2C_SlaveReceiver
                - I2C通信
        - arduino_Joystick_ESC
            - ジョイスティックによるESC制御
        - arduino_joystick_joystick
            - ジョイスティック制御
        - arduino_joystick_servo
            - ジョイスティックによるサーボ制御
        - arduino_joystick_servoESC
            - ジョイスティックによるサーボとESCの制御
        - arduino_MMA7660FC_MMA7660FC
            - 加速度センサー測定
        - arduino_optical_LED
            - 光センサーによるLED制御
        - arduino_optical_TA7291P
            - 光センサーによるモーター制御
        - arduino_Ultrasonic_Ultrasonic
            - 超音波センサ測定
        - arduino_USBPTP_THETA
            - PTPコマンドによるRICOH THETA制御、ディスクリプタの取得に失敗する
        - arduino_wii_wii
            - USB Host Shield & BluetoothドングルによるWiiリモコン制御
        - ESP32_Blynk_BLE
            - [Blynk](https://www.blynk.cc/)ジョイスティックによるESP32制御
        - ESP32_Fixed_I2C
            - ESP32でI2C通信試したが対応してなかった
        - ESP32_Fixed_SimpleWiFiServer
            - ESP32用
        - ESP32_WiFiCLient_THETA
            - ESP32でTHETA接続と静止画撮影
        - ESP32_WiFiScan_THETA
            - ESP32用
- ESP-IDF
    - esp32_theta
        - 2017/12/16-17に開催された[NT加賀](http://wiki.nicotech.jp/nico_tech/index.php?NT%E5%8A%A0%E8%B3%802017)に出展した作品のESP-IDF projectである
        - RICOH THETAとWi-Fi接続し、I2Cから'A'を受け取ると、OSC V2.1 WebAPIでTakePicture(静止画撮影)を送信する
    - モジュール動作確認用各種スケッチ
        - gatt_client
            - RICOH THETA V とBLE接続し、以下のコマンドを送信する(未完成)
                - Bluetooth Control Command の AUTH_BLUETOOTH_DEVICE に UUIDをwrite
                - Shooting Control Command の Take Picture に 1をwrite
        - gatt_security_client
            - 作成中
        - hello_world
            - Hello worldサンプルプロジェクト
        - http_request
            - RICOH THETA V とWiFi接続し、以下のコマンドを送信する(未完成)
                - camera.takePicture(静止画撮影)
                - _bluetoothPowerをON
                - camera._setBluetoothDeviceでUUIDを登録

## Demo

## VS.

## Requirement

## Usage

## Install

## Contribution

## Licence

- 全て Mitライセンスとする
    - Hastler & Wii_servo_ESC & ESP32_Blynk_Roadster Mitライセンス
    - その他Arduino IDEに付属するスケッチ例(LGPL)から修正したスケッチはMitライセンス
    - [Blynk](https://www.blynk.cc/)に付属するサンプル(Mitライセンス)から作成したスケッチはMitライセンス
    - USB Host Shield用スケッチ例(LGPL)から修正したスケッチはMitライセンス
    - ESP-IDF用サンプルprojectから修正したprojectもMitライセンス

## Author

- igapon(igapon@gmail.com)
