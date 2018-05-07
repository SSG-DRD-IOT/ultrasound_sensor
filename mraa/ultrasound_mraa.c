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

/* standard headers */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/gpio.h"

#define TRIG_PIN 15 //MRAA NO 15, PIN 15 on UP2 board
#define ECHO_PIN 13 //MRAA NO 13, PIN 13 on UP2 board

#define HIGH 1
#define LOW 0

#define BUF 8
#define MAX_BUF 256

const int trigPinDuration = 10; // trigger duration
const int soundBurstDuration = 8; //ms when burst sent

const double distanceConstant = 59.0;

const double minDistance = 2.00; //cms
const double maxDistance = 400.00; //cms


int main() {
    mraa_result_t status = MRAA_SUCCESS;

    //Set GPIO PIN MODES
    int trig_pin = mraa_gpio_init(TRIG_PIN);

    if (trig_pin == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", TRIG_PIN);
        return EXIT_FAILURE;
    }

    int echo_pin = mraa_gpio_init(ECHO_PIN);

    if (echo_pin == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", ECHO_PIN);
        return EXIT_FAILURE;
    }

    status = mraa_gpio_dir(trig_pin, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    status = mraa_gpio_dir(echo_pin, MRAA_GPIO_IN);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    char ch;

    //start distance calculation
    do{
        printf("Calculating New Distance .... \n");
        mraa_gpio_write(trig_pin, 1);
        usleep(10);
        mraa_gpio_write(trig_pin, 0);

        struct timeval start;
        struct timeval end;

        int echo = 0;
        int counter = 0;
        gettimeofday(&start, NULL);

        long cycleLength = (1000000 * start.tv_sec) + start.tv_usec + 70000;
        long sampleTime = 0;

        while (sampleTime < cycleLength) {
            echo = mraa_gpio_read(echo_pin);
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
        if ((int) distance > maxDistance) {
            printf("%f centimeters\n", maxDistance);
        } else if ((int) distance < minDistance) {
            printf("%f centimeters\n", maxDistance);
        } else {
            printf("%f centimeters\n", distance);
        }

        sleep(1);//2 sec delay

        ch = getchar();
    }while(ch != 'q' && ch != 'Q');

    /* release gpio's */
    status = mraa_gpio_close(trig_pin);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    /* close GPIO */
    status = mraa_gpio_close(echo_pin);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    return 0;
}
