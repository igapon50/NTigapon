
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
        - WiiリモコンのUP(ステアリング左)/DOWN(ステアリング右)/1(加速)/2(減速)ボタンを使用する
    - Wii_servo_ESC
        - 2017/12/16-17に開催されるNT加賀に出展する作品のArduino sketchである
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
