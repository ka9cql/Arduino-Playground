# Arduino-Playground
Some simple Arduino projects (mostly as experiments)

First program is a Burglar Alarm that uses a PIR (Passive Infared) motion sensor to detect intruders. It has a keypad and two different passcodes to enable and disable the alarm. It is inspired from code found here - https://create.arduino.cc/projecthub/thehack904/motion-sensing-alarm-with-keypad-password-53f05c (CAUTION: The circuit diagram at that URL is inaccurate) - with LCD display code grabbed from here - https://create.arduino.cc/projecthub/paolo_cravero/arduino-burglar-b445d5.  This example still needs switch-debouncing, but is otherwise considered real-world usable.

I tried, but was unsuccessful at adding an IR-based keypad due to internal timer conflicts, so this project is now considered "done", as-is.
