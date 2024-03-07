#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#define SCL 1 // Define GPIO for i2c0 SCL Line
#define SDA 0 // Define GPIO for i2c0 SDA Line

#define addr 0x48 // touchscreen target address both A0 and A1 are held low making address 1001000

void setup()
{
  stdio_init_all();

  // Setup GPIOs to work with I2C
  gpio_init(SCL);
  gpio_init(SDA);
  gpio_set_pulls(SCL, 1, 0);
  gpio_set_pulls(SDA, 1, 0);

  gpio_set_function(SCL, GPIO_FUNC_I2C);
  gpio_set_function(SDA, GPIO_FUNC_I2C);

  i2c_init(i2c0, 100000); // Initialize i2c0 and set the speed to 100 kb/s standard mode

  uint8_t snd[1] = {0xB0}; // 10110000 pg.25
}

void loop()
{
  uint8_t snd[1] = {0xB0}; // 10110000 pg.25
  uint8_t x_pos[1] = {0xC0};
  uint8_t y_pos[1] = {0xD0};
  uint8_t x[2] = {0, 0};
  uint8_t y[2] = {0, 0};

  float pos_x = 0;  // creates a variable that reads x position
  float pos_y = 0;  // creates a variable that reads y position

  i2c_write_blocking(i2c0, addr, snd, 1, 1); // writes command register to the target
  i2c_write_blocking(i2c0, addr, x_pos, 1, 1);
  i2c_read_blocking(i2c0, addr, x, 2, 0); // reads 1 byte long x value from the target
  //i2c_write_blocking(i2c0, addr, snd, 1, 1); // writes command register to the target
  i2c_write_blocking(i2c0, addr, y_pos, 1, 1); // writes command register to the target
  i2c_read_blocking(i2c0, addr, y, 2, 0); // reads 1 byte long y values from target

  // solve for positiion
  pos_x = (((uint16_t)x[0] << 4) | (x[1] >> 4))*(3.3/4096);
  pos_y = (((uint16_t)y[0] << 4) | (y[1] >> 4))*(3.3/4096);

  printf("%.2f\t%.2f\n", pos_x, pos_y);
  sleep_ms(50); 
}

int main()
{
  setup();
  sleep_ms(5000);
  printf("x\t\ty");
  while(true)
  {
    loop();
  }
}
