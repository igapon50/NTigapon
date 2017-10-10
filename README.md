
# Title

- NTigapon

===

## Overview
Arduino sketch and ESP-IDF project.


## Description

- arduino
    - Hastler
        - 2017/7/8-9に開催されたNT金沢に出展した作品のArduino sketchである
        - 市販品の1/20スケールラジコンカー[Hustler](https://www.amazon.co.jp/dp/B072JSL6Z9/ref=pe_2107282_266464282_TE_3p_dp_1)を改造する
        - Hustlerのモーター×2とLED×2に光センサーを追加し、Arduinoで制御する
    - Wii_servo_ESC
        - 2017/12/16-17に開催されるNT加賀に出展する作品のArduino sketchである
        - 市販品の1/10スケールラジコンカー[ROADSTER](http://www.tamiya.com/japan/products/57891/index.html)を改造する
        - ROADSTERのサーボ×1とESC×1に超音波距離センサー×2を追加し、Arduinoで制御する
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
            - SPI通信
- ESP-IDF
    - gatt_client
        - RICOH THETA V とBLE接続し、以下のコマンドを送信する
            - Bluetooth Control Command の AUTH_BLUETOOTH_DEVICE に UUIDをwrite
            - Shooting Control Command の Take Picture に 1をwrite
    - gatt_security_client
        - 作成中
    - hello_world
        - Hello worldサンプルプロジェクト
    - http_request
        - RICOH THETA V とWiFi接続し、以下のコマンドを送信する
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
