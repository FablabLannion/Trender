# Trender

The Trender is an objet translating different types of information into light.

*Read this in other languages: [English](README.md), [Français](README.fr.md), [Brezhoneg](README.bzh.md)*

## Table of Contents
  - [Get started](#get-started)
  - [Modes](#modes)
    - [Time keeper](#time-keeper)
    - [Jenkins](#jenkins)
    - [rainbow](#rainbow)
  - [FAQ](#faq)

## Get started
The procedure to setup your Trender is:
  * Connect your Trender to a power supply through the USB cable
  * On your mobile phone, select the Wifi network named TRENDER-XXXX
  * On your mobile phone, open a web browser and enter the url http://192.168.4.1

You should see the following screen:

![Main screen](/screenshots/main.png)

 1. Configure WiFi
 2. Configure WiFi (No Scan)
 3. Info
 4. Reset

### Configure WiFi

The Configure WiFi mode allows to connect the Trender to the Web.
The Trender will connect automatically if it has been already configured.
It is possible to scan all the access points.

### Configure WiFi (No Scan)


### Info

This menu provides Network information (SSID, IP, MAC).

### Reset

This menu can be used to perform a factory reset. All the previous parameters
(especially the WiFi configuration) are cleaned.


## Modes

The Trender supports several modes. You can use it as:
 * Demo
 * Timekeeper
 * Notification system (twitter, jenkins)

By default the Trender is in Notification mode.

You can switch from Notification to Timekeeper mode by putting *1* in the mode
window. A green light will blink to indicate that the mode has been changed.

Once in timekeeper mode, you may come back to notification mode by putting *0*
in the mode window.

### Timekeeper

A timekeeper is often referred to as a time clock, which tracks time. The
Trender allows to set 3 fully customizable thresholds.

From the WiFi configure menu, once you set *1* in the mode window, come back to
the main page. A new menu will be displayed.

![Color](/screenshots/tk.png)

Select the menu **TIMEKEEPER CONFIGURATION**

#### Configuration

You can configure:
 * the duration
 * 3 intermediate steps

![Color](/screenshots/color.png)


### Notification system

The Trender may translate web information into light.
You must configure a web public access point. You must select the SSID and
enter the password. Then you must select the ThingSpeak channel ID.

![Color](/screenshots/ThingSpeak.png)


### Demo

The Demo mode can be started from the Timekeeper menu.
Select the submenu Color the tick rainbow or choose the color you want.

## FAQ
