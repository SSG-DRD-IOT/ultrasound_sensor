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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "upm_utilities.h"
#include "hcsr04.h"

#define TRIG_PIN 15 //MRAA NO 15, PIN 15 on UP2 board
#define ECHO_PIN 13 //MRAA NO 13, PIN 13 on UP2 board

const double minDistance = 2.00; //cms limits of sensor
const double maxDistance = 400.00; //cms

int main() {
    hcsr04_context dev = hcsr04_init(TRIG_PIN, ECHO_PIN);

    if(dev == NULL) {
        printf("Unable to intialize the sensor\n");
        return 0;
    }

    double distance;

    char ch;

    //start distance calculation
    do{
        printf("Calculating New Distance .... \n");

        distance = hcsr04_get_distance(dev, HCSR04_CM);

        printf("New Distance: ");
        if ( distance > maxDistance) {
            printf("%f centimeters\n", maxDistance);
        } else if ( distance < minDistance) {
            printf("%f centimeters\n", maxDistance);
        } else {
            printf("%f centimeters\n", distance);
        }

        //delay 2 seconds
        sleep(2);

        ch = getchar();

    }while(ch != 'q' && ch != 'Q');

    return 0;
}
