#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdlib.h>

// Control pins, active low (N at the end of name) means they are "on" when at 0 and "off" at 1
#define LCD_RDN 8   // Controls whether to read signal or not
#define LCD_WRN 9   // Tells the display if we want to write or not
#define LCD_RSN 10  // 0 to send a command and 1 to send data
#define LCD_CSN 11  // On and off switch for communcation, activates or deactives the display controller
#define LCD_RSTN 12 // Reset the display controller

// Data bus pins (D0-D7 are GPIO 0-7)
#define LCD_DATA_START 0
#define LCD_DATA_END 7

// Display dimensions and colors
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F

void init_gpio()
{
    // Initalize pins
    gpio_init(LCD_RDN);
    gpio_init(LCD_WRN);
    gpio_init(LCD_RSN);
    gpio_init(LCD_CSN);
    gpio_init(LCD_RSTN);

    gpio_set_dir(LCD_RDN, GPIO_OUT);
    gpio_set_dir(LCD_WRN, GPIO_OUT);
    gpio_set_dir(LCD_RSN, GPIO_OUT);
    gpio_set_dir(LCD_CSN, GPIO_OUT);
    gpio_set_dir(LCD_RSTN, GPIO_OUT);

    // Set state 0 = active, 1 = active
    gpio_put(LCD_RDN, 1);
    gpio_put(LCD_WRN, 1);
    gpio_put(LCD_RSN, 1);
    gpio_put(LCD_CSN, 1);
    gpio_put(LCD_RSTN, 1);

    for (int i = LCD_DATA_START; i <= LCD_DATA_END; i++)
    {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
        gpio_put(i, 0); // Start with data pins at 0
    }
}

void write_byte(uint8_t data, bool is_data)
{

    // True for data false for command
    gpio_put(LCD_RSN, is_data);
    // Enable communication
    gpio_put(LCD_CSN, 0);

    // Set data pins according to our byte
    for (int i = LCD_DATA_START; i <= LCD_DATA_END; i++)
    {
        bool bit = (data >> i) & 0x01;
        gpio_put(i, bit);
    }

    // Generate write pulse
    gpio_put(LCD_WRN, 0);
    sleep_us(1);
    gpio_put(LCD_WRN, 1);

    // Disable communication
    gpio_put(LCD_CSN, 1);
}

void write_command(uint8_t command)
{
    write_byte(command, false);
}

void write_data(uint8_t data)
{
    write_byte(data, true);
}

void reset_sequence()
{

    // Activate reset sequence
    gpio_put(LCD_RSTN, 1);
    sleep_ms(5);
    gpio_put(LCD_RSTN, 0);
    sleep_ms(15);
    gpio_put(LCD_RSTN, 1);
    sleep_ms(15);
}

// Based on Ted Rossin's initialization sequence
void display_init()
{
    reset_sequence();

    write_command(0x01); // Software reset
    sleep_ms(5);

    write_command(0xCF); // Power control B
    write_data(0x00);
    write_data(0x81);
    write_data(0X30);

    write_command(0xED); // Power on sequence
    write_data(0x64);
    write_data(0x03);
    write_data(0X12);
    write_data(0X81);

    write_command(0xE8); // Driver timing control A
    write_data(0x85);
    write_data(0x10);
    write_data(0x78);

    write_command(0xCB); // Power control A
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);

    write_command(0xF7); // Pump ratio control
    write_data(0x20);

    write_command(0xEA); // Driver timing control B
    write_data(0x00);
    write_data(0x00);

    write_command(0xC0); // Power Control 1
    write_data(0x21);

    write_command(0xC1); // Power Control 2
    write_data(0x11);

    write_command(0xC5); // VCOM Control 1
    write_data(0x3F);
    write_data(0x3C);

    write_command(0xC7); // VCOM Control 2
    write_data(0XB5);

    write_command(0x36); // Memory Access Control
    write_data(0x48);

    write_command(0x3A); // Pixel Format Set
    write_data(0x55);    // 16-bit color

    write_command(0xB1); // Frame Rate Control
    write_data(0x00);
    write_data(0x18);

    write_command(0xB6); // Display Function Control
    write_data(0x0A);
    write_data(0xA2);

    write_command(0xF2); // Enable 3G
    write_data(0x00);

    write_command(0x26); // Gamma Set
    write_data(0x01);

    write_command(0xE0); // Positive Gamma Correction
    write_data(0x0F);
    write_data(0x23);
    write_data(0x1F);
    write_data(0x0B);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4B);
    write_data(0XA8);
    write_data(0x3B);
    write_data(0x0A);
    write_data(0x14);
    write_data(0x06);
    write_data(0x10);
    write_data(0x09);
    write_data(0x00);

    write_command(0XE1); // Negative Gamma Correction
    write_data(0x00);
    write_data(0x1C);
    write_data(0x20);
    write_data(0x04);
    write_data(0x10);
    write_data(0x08);
    write_data(0x34);
    write_data(0x47);
    write_data(0x44);
    write_data(0x05);
    write_data(0x0B);
    write_data(0x09);
    write_data(0x2F);
    write_data(0x36);
    write_data(0x0F);

    write_command(0x11); // Exit Sleep Mode
    sleep_ms(120);

    write_command(0x29); // Display ON
    sleep_ms(100);
}

void set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{

    // Set column address
    write_command(0x2A);
    write_data(x1 >> 8);
    write_data(x1 & 0xFF);
    write_data(x2 >> 8);
    write_data(x2 & 0xFF);

    // Set row address
    write_command(0x2B);
    write_data(y1 >> 8);
    write_data(y1 & 0xFF);
    write_data(y2 >> 8);
    write_data(y2 & 0xFF);

    // Prepare to write memory
    write_command(0x2c);
}

void draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
    {
        printf("Trying to draw a pixel out of bounds");
        return;
    }
    set_window(x, y, x, y);
    write_data(color >> 8);
    write_data(0xFF);
}

// Draws a line with the help of Bresenham's algorithm
void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    // Out of bounds
    if (y1 >= DISPLAY_HEIGHT || y2 >= DISPLAY_HEIGHT ||
        y1 < 0 || y2 < 0)
    {
        printf("Out of bounds on height");
        return;
    }
    if (x1 >= DISPLAY_WIDTH || x2 >= DISPLAY_WIDTH ||
        x1 < 0 || x2 < 0)
    {
        printf("Out of bounds on width");
        return;
    }

    if (x1 == x2)
    {
        // Vertical line
        int16_t start_y = (y1 <= y2) ? y1 : y2;
        int16_t end_y = (y1 >= y2) ? y1 : y2;

        for (int16_t i = start_y; i <= end_y; i++)
        {
            draw_pixel(x1, i, color);
        }
    }
    else if (y1 == y2)
    {
        // Horizontal line
        int16_t start_x = (x1 <= x2) ? x1 : x2;
        int16_t end_x = (x1 >= x2) ? x1 : x2;

        for (int16_t i = start_x; i <= end_x; i++)
        {
            draw_pixel(i, y1, color);
        }
    }
    else
    {
        // Delta between each point
        uint16_t dx = abs(x2 - x1);
        uint16_t dy = abs(y2 - y1);

        // Direction of movement
        int16_t sx = (x1 < x2) ? 1 : -1;
        int16_t sy = (y1 < y2) ? 1 : -1;
        int16_t err = (dx > dy ? dx : -dy) / 2;
        int16_t e2;

        if (dx > dy)
        {
            while (1)
            {
                draw_pixel(x1, y1, color);

                if (x1 == x2 && y1 == y2)
                    break;

                e2 = err;
                if (e2 > -dx)
                {
                    err -= dy;
                    x1 += sx;
                }
                if (e2 < dy)
                {
                    err += dx;
                    y1 += sy;
                }
            }
        }
    }
}

void draw_rectangle(uint16_t posX, uint16_t posY, uint16_t size, uint16_t color, bool fill)
{
    // Accomodate for different width and heigth
    // Replace size with two new params that accomodate for width and heigth

    uint16_t endX = posX + size;
    uint16_t endY = posY + size;

    if (fill == true)
    {
        for (size_t i = posY; i <= endY; i++)
        {
            draw_line(posX, i, endX, i, color);
        }
    }
    else
    {
        // Top line
        draw_line(posX, posY, endX, posY, color);
        // Right line
        draw_line(endX, posY, endX, endY, color);
        // Bottom line
        draw_line(posX, endY, endX, endY, color);
        // Left line
        draw_line(posX, posY, posX, endY, color);
    }
}

int main()
{
    stdio_init_all();
    printf("Starting basic pin setup and display init...\n");

    init_gpio();

    sleep_ms(5000);
    printf("Sleep over!");

    display_init();

    set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    draw_rectangle(20, 20, 5, GREEN, true);
    draw_rectangle(40, 60, 20, GREEN, true);
    draw_rectangle(100, 100, 5, BLUE, false);
    draw_rectangle(150, 200, 30, BLUE, false);


    while (true)
    {
        printf("Sleeping...\n");
        sleep_ms(1000);
    }

    return 0;
}