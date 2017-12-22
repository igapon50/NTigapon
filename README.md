
# Title

- NTigapon

===

## Overview
Arduino sketch and ESP-IDF project.


## Description

- arduino
    - Hastler
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
    - Wii_servo_ESC
        - 2017/12/16-17に開催される[NT加賀](http://wiki.nicotech.jp/nico_tech/index.php?NT%E5%8A%A0%E8%B3%802017)に出展する作品のArduino sketchである
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
    - モジュール動作確認用各種スケッチ
        - ESC
            - ジョイスティックによるESC制御
        - joystick
            - ジョイスティック制御
        - MMA7660FC
            - 加速度センサー測定
        - motor
            - モーター制御
        - optical
            - LED制御
        - servo
            - ジョイスティックによるサーボ制御
        - servo_ESC
            - ジョイスティックによるサーボとESCの制御
        - UltrasonicDisplayOnTerm
            - 超音波センサ測定
        - UltrasonicDisplayOnTerm_HS-SR04
            - HS-SR04超音波センサ二つ測定
        - Wii
            - USB Host Shield & BluetoothドングルによるWiiリモコン制御
        - wire
            - I2C通信
- ESP-IDF
    - esp32_theta
        - RICOH THETAとWi-Fi接続し、I2Cから'A'を受け取ると、OSC V2.1 WebAPIでTakePicture(静止画撮影)を送信する
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
    - Hastler & Wii_servo_ESC Mitライセンス
    - その他Arduino IDEに付属するスケッチ例(LGPL)から修正したスケッチはMitライセンス
    - USB Host Shield用スケッチ例(LGPL)から修正したスケッチはMitライセンス
    - ESP-IDF用サンプルprojectから修正したprojectもMitライセンス

## Author

- igapon(igapon@gmail.com)
