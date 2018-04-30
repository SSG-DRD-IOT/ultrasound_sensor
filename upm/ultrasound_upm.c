
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "upm.h"
#include "upm_utilities.h"
#include "hcsr04.h"

#define TRIG_PIN 431 //MRAA NO 7, PIN 15 on UP2 board
#define ECHO_PIN 432 //MRAA NO 13, PIN 13 on UP2 board

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
        upm_delay(2);

        ch = getchar();

    }while(ch != 'q' && ch != 'Q');

    return 0;
}
