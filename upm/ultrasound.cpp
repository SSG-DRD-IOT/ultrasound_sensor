
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "upm_utilities.h"
#include "hcsr04.h"

#define TRIG_PIN 431 //MRAA NO 7, PIN 15 on UP2 board
#define ECHO_PIN 432 //MRAA NO 13, PIN 13 on UP2 board

int main() {
    hcsr04_context dev = hcsr04_init(TRIG_PIN, ECHO_PIN);

    if(dev == NULL) {
        printf("Unable to intialize the sensor\n");
        return 0;
    }

    double distance;
    while(1) {
        distance = hcsr04_get_distance(dev, HCSR04_CM);
        printf("Distance detected: %f\n", distance);
        upm_delay(1);
    }

    return 0;
}
