# Alarm Clock Lamp

## Pinout

According to [Pinout](https://www.heise.de/developer/imgs/06/2/3/9/9/2/8/7/Bluepillpinout-4974006ee90b729a.gif)

| Pin | Connection |  
|-----|------------|
| B12 | GPIO Lautsprecher GND |
| A8 | PWM LED warm (TIM1) |
| A9 | PWM LED kalt (TIM1) |
| A11 | GPIO On/Off (EXTI) |
| A12 | GPIO All-off (EXTI) |
| A15 | GPIO Buzzer |
|-----|------------------------------------|
| R | Reset Button |
| B11 | SDA TEA |
| B10 | SCL TEA |
| A7 | MOSI |
| A6 | MISO |
| A5 | SPI CLK |
| A4 | CSN nRF |
| A3 | IRQ nRF (EXTI) |
| A2 | GPIO CE nRF |
|-----|------------------------------------|

TEA BUSMODE --> GND
