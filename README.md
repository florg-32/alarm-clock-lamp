# Alarm Clock Lamp

## Todos

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

### nRF control procedure
- Power on
- wait 100ms
- Write 0x03: 0x01 (Address width = 3byte)
- Write 0x0A: 0xdedad0 (RX Address)
- Write 0x11: 0x08 (Payload length)
- Write 0x1D: 0x01 (Feature NOACK)
- Write 0x00: 0x03 (PWR_UP, PRIM_RX)
- pull CE high
- wait 1.5ms + 130µs settling --> RX Mode
