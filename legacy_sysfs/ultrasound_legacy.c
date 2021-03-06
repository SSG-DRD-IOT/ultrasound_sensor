/*
 * Author: Onur Dundar <onurx.dundar@Intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/time.h>

#define TRIG_PIN 431 //MRAA NO 7, PIN 15 on UP2 board
#define ECHO_PIN 432 //MRAA NO 13, PIN 13 on UP2 board

#define HIGH 1
#define LOW 0

#define INPUT "in"
#define OUTPUT "out"

#define BUF 8
#define MAX_BUF 256

const int TriggerDuration = 10; // trigger duration

const double distanceConstant = 59.0;// uS constant as datasheet pointed

const double minDistance = 2.00; //cms limits of sensor
const double maxDistance = 400.00; //cms

/**
 * Export Given GPIO
 * @param gpio_num
 * @return
 */
int gpio_export(int gpio_num) {
    //Device File Path Declarations
    const char* gpio_export = "/sys/class/gpio/export";
    //Device File Declarations
    int fd_x = 0, g_err = -1;
    //Buffer
    char g_buf[BUF];
    fd_x = open(gpio_export, O_WRONLY);
    if (fd_x < 0) {
        printf("Couldn't get export FD\n");
        return g_err;
    }
    //Export GPIO Pin
    sprintf(g_buf, "%d", gpio_num);
    if (write(fd_x, g_buf, sizeof(g_buf)) == g_err) {
        printf("Couldn't export GPIO %d\n", gpio_num);
        close(fd_x);
        return g_err;
    }
    close(fd_x);
    return 0;
}

/**
 * GPIO Mode Set Method
 * @param gpio_num
 * @param mode
 * @return
 */
int gpio_set_mode(int gpio_num, const char* mode) {
    //Device Direction File Path Declarations
    const char* gpio_direction_path = "/sys/class/gpio/gpio%d/direction";
    //Device File Declarations
    int fd_d = 0, g_err = -1;
    //Buffers
    char pindirection_buf[MAX_BUF];
    char d_buf[BUF];
    //Set pin number and set gpio path
    if (sprintf(pindirection_buf, gpio_direction_path, gpio_num) < 0) {
        printf("Can't create pin direction file path\n");
        return g_err;
    }
    //Open GPIO Direction File
    fd_d = open(pindirection_buf, O_WRONLY);
    //If GPIO doesn't exist then export gpio pins
    if (fd_d < 0) {
        if (gpio_export(gpio_num) < 0) {
            return g_err;
        }
        fd_d = open(pindirection_buf, O_WRONLY);
        if (fd_d <= 0) {
            printf("Couldn't get direction File for pin %d\n", gpio_num);
            return g_err;
        }
    }
    sprintf(d_buf, mode);
    if (write(fd_d, d_buf, sizeof(d_buf)) == g_err) {
        printf("Couldn't set direction for pin %d\n", gpio_num);
        return g_err;
    }
    close(fd_d);
    return 0;
}



/**
 * GPIO Set Value
 * @param gpio_num integer gpio number in linux
 * @param value integer LOW/HIGH to set
 * @return success/error
 */
int gpio_set_value(int gpio_num, int value) {
    //Device Direction File Path Declarations
    const char* gpio_value_path = "/sys/class/gpio/gpio%d/value";
    //Device File Declarations
    int fd_v = 0, g_err = -1;
    //Buffers
    char pinvalue_buf[MAX_BUF];
    char v_buf[BUF];
    //Set pin number and set gpio path
    if (sprintf(pinvalue_buf, gpio_value_path, gpio_num) < 0) {
        printf("Can't create pin direction file path\n");
        return g_err;
    }
    //Open GPIO Value File
    fd_v = open(pinvalue_buf, O_WRONLY);
    //If GPIO doesn't exist then export gpio pins
    if (fd_v < 0) {
        if (gpio_export(gpio_num) < 0) {
            return g_err;
        }
        fd_v = open(pinvalue_buf, O_WRONLY);
        if (fd_v <= 0) {
            printf("Couldn't get value File for pin %d\n", gpio_num);
            return g_err;
        }
    }
    sprintf(v_buf, "%d", value);
    if (write(fd_v, v_buf, sizeof(v_buf)) == g_err) {
        printf("Couldn't set value for pin %d\n", gpio_num);
        return g_err;
    }
    close(fd_v);
    return 0;
}

/**
 * Get current value of GPIO
 * @param gpio_num integer gpio number in linux
 * @return HIGH/LOW
 */
int gpio_get_value(int gpio_num) {
    //Device Direction File Path Declarations
    const char* gpio_value_path = "/sys/class/gpio/gpio%d/value";
    //Device File Declarations
    int fd_v = 0, g_err = -1;
    //Buffers
    char pinvalue_buf[MAX_BUF];
    char v_buf[BUF];
    //Set pin number and set gpio path
    if (sprintf(pinvalue_buf, gpio_value_path, gpio_num) < 0) {
        printf("Can't create pin direction file path\n");
        return g_err;
    }
    //Open GPIO Value File
    fd_v = open(pinvalue_buf, O_RDONLY);
    //If GPIO doesn't exist then export gpio pins
    if (fd_v < 0) {
        if (gpio_export(gpio_num) < 0) {
            return g_err;
        }
        fd_v = open(pinvalue_buf, O_RDONLY);
        if (fd_v <= 0) {
            printf("Couldn't get value File for pin %d\n", gpio_num);
            return g_err;
        }
    }
    if (read(fd_v, v_buf, 1) == g_err) {
        printf("Couldn't get value for pin %d\n", gpio_num);
        return g_err;
    }
    close(fd_v);
    return atoi(v_buf);
}

int main() {
    //Set GPIO PIN MODES
    gpio_set_mode(TRIG_PIN, OUTPUT);
    gpio_set_mode(ECHO_PIN, INPUT);

    char ch;

    //start distance calculation
    do{
        printf("Calculating New Distance .... \n");
        //Set Trigger Pin HIGH
        gpio_set_value(TRIG_PIN, HIGH);
        //wait for trigger duration
        usleep(TriggerDuration);
        //Set Trigger Pin LOW
        gpio_set_value(TRIG_PIN, LOW);

        struct timeval start;
        struct timeval end;

        int echo = 0;
        int counter = 0;
        gettimeofday(&start, NULL);

        long cycleLength = (1000000 * start.tv_sec) + start.tv_usec + 70000;
        long sampleTime = 0;

        //wait till echo pin gets low
        while (sampleTime < cycleLength) {
            echo = gpio_get_value(ECHO_PIN);
            if (echo == 1 && counter == 0) {
                gettimeofday(&start, NULL);
                counter++;
            } else if (echo == 0 && counter == 1) {
                gettimeofday(&end, NULL);
                break;
            } else {
                sampleTime = (1000000 * start.tv_sec) + start.tv_usec;
            }
        }

        double s = (double) start.tv_sec * 1000000 + (double) start.tv_usec;
        double e = (double) end.tv_sec * 1000000 + (double) end.tv_usec;

        double distance = (e - s) / distanceConstant;

        printf("New Distance: ");
        if ( distance > maxDistance) {
            printf("%f centimeters\n", maxDistance);
        } else if ( distance < minDistance) {
            printf("%f centimeters\n", maxDistance);
        } else {
            printf("%f centimeters\n", distance);
        }

        //wait 2 seconds
        sleep(2);

        ch = getchar();
    }while(ch != 'q' && ch != 'Q');

    return 0;
}
