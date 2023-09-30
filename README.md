# MediBox

Medibox is a sophisticated device designed to assist users in effectively managing their medication schedules while ensuring optimal storage conditions for sensitive medicines. This GitHub repository contains the source code and documentation for the Medibox project.

## Features
Medibox offers the following key features:

** User-Friendly Menu Interface:
Easily configure device settings, including time zone, alarms, and alarms disabling.

** Time Zone Setting:
Set the device's time zone by specifying the offset from UTC for accurate local time display.

** Multiple Alarms:
Configure up to three alarms to remind users to take their medications at specified times.

** Alarm Indications:
Receive timely alarms with appropriate indications using visual and audible alerts.

** Alarm Stop Button:
Quickly silence alarms using the dedicated stop button.

** NTP Time Synchronization:
Fetch accurate local time from NTP servers over Wi-Fi to ensure precise timekeeping.

** Temperature and Humidity Monitoring:
Continuously monitor temperature and humidity levels, providing warnings when limits are exceeded for medication storage safety.

** Light Intensity Measurement:
Measure real-time light intensity using a Light Dependent Resistor (LDR) and visualize it on the Node-RED dashboard.

** Dynamic Light Regulation:
Maintain optimal storage conditions by adjusting a shaded sliding window with a servo motor, adapting to varying lighting conditions.

** Customizable Settings:
Tailor the system to your medication needs by adjusting parameters like the minimum angle and controlling factor for the shaded sliding window via the Node-RED dashboard.
