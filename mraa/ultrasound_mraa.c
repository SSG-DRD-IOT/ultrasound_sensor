
/* standard headers */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/gpio.h"

#define TRIG_PIN 431 //MRAA NO 7, PIN 15 on UP2 board
#define ECHO_PIN 432 //MRAA NO 13, PIN 13 on UP2 board

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
    int trig_pin = mraa_gpio_init(triggerPin);

    if (trig_pin == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", TRIG_PIN);
        return EXIT_FAILURE;
    }

    int echo_pin = mraa_gpio_init(echoPin);

    if (echo_pin == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", ECHO_PIN);
        return EXIT_FAILURE;
    }

    status = mraa_gpio_dir(TRIG_PIN, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    status = mraa_gpio_dir(ECHO_PIN, MRAA_GPIO_IN);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    char ch;

    //start distance calculation
    do{

        mraa_gpio_write(dev->trigPin, 1);
        upm_delay_us(10);
        mraa_gpio_write(dev->trigPin, 0);

        struct timeval start;
        struct timeval end;

        int echo = 0;
        int counter = 0;
        gettimeofday(&start, NULL);

        long cycleLength = (1000000 * start.tv_sec) + start.tv_usec + 70000;
        long sampleTime = 0;

        while (sampleTime < cycleLength) {
            echo = mraa_gpio_read(ECHO_PIN);
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

        printf("Start Time us :%f\n", s);
        printf("End Time us :%f\n", e);

        printf("New Distance: ");
        if ((int) distance > maxDistance) {
            printf("%f centimeters\n", maxDistance);
        } else if ((int) distance < minDistance) {
            printf("%f centimeters\n", maxDistance);
        } else {
            printf("%f centimeters\n", distance);
        }

        upm_delay(2);//2 sec delay

        ch = getchar();
    }while(ch != 'q' && ch != 'Q');

    /* release gpio's */
    status = mraa_gpio_close(TRIG_PIN);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    /* close GPIO */
    status = mraa_gpio_close(ECHO_PIN);
    if (status != MRAA_SUCCESS) {
        mraa_result_print(status);
        return EXIT_FAILURE;
    }

    return 0;
}
