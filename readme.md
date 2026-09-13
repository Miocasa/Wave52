# Wave52

Project based on nrf52480 (xiao seeed nrf52480 sense), bme280 climate sensor and mini ds3231 rtc module to get actual
time and alarm interuption.   
As display I have used Weact 4.2, but i planing to make code full compatible with another
displays, (you can see it in `DisplayManager.h` and `display_detection.h`)

### TODO:

- First stage tasks
    - [*] Make widget system
    - [*] Load and save screen presets to `InternalFs` or `QSPIFileSystem`
    - [ ] Loading screen from `config.bin` file or default for case when it unavailable.
    - [ ] Make ble connection and application or web app for a start
    - [ ] Ble connection and services
- TextBox
    - [ ] Parsing string for arguments in TextBox widget.
    - [ ] Byte to byte string output to match given space
- Low priority
    - [ ] Make prettier `readme.md`

### Support

If you interested in project you can support me with start this repository or contributing. I'm open for suggests.

> In code i used some code references and platform.ini files from [Volna42](https://github.com/NC22/Volna42BW), made by
[nradiowave](https://nradiowave.ru/).   