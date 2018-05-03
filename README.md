## Programming Sensors with Legacy C vs MRAA, UPM

This tutorial tries to show the importance of having an hardwared abstraction layer and sensor library like MRAA and UPM to speed up the prototyping, development and deployment processes.

I wanted to keep things as simple as possible with using a sensor with GPIO I/O, an ultrasound sensor HC-SR04. HC-SR04 sensor is a widely used and can be easily accessed from many of the sensor development kits for makers.

Let's understand our sensor first. HC-SR04 sensor has 4 pins, VCC, GND, TRIG and ECHO pins. VCC pin is for 5V input and GND pin is used for ground connection. HC-SR04 uses sound and its echo timings to measure distance of an obstacle front of the sensor.

Connecting HC-SR04 to UP2 Board Pins:
https://www.electroschematics.com/8902/hc-sr04-datasheet/

**Image-1: HC-SR04 Sensor**

![Image of HC-SR04](https://github.com/odundar/ultrasound_sensor/blob/master/resources/hcsr04.png)

**Image-2: Electronics of HC-SR04 Sensor**

![Image of HC-SR04 DS](https://github.com/odundar/ultrasound_sensor/blob/master/resources/hcsr04-ds.png)

**Image-3: Timing of HC-SR04 Sesnsor**
![Image of HC-SR04 TTL](https://github.com/odundar/ultrasound_sensor/blob/master/resources/hcsr04-ttl.png)

As we seen from the datasheet. 
* 10 uS of TTL signal triggers a 8 uS sound burst.
* Then, sensor receives echoed sound and changes state of Echo pin signal.
* Length of Echo pin's signal's change hint us the distance in centimeters or inches.
* As datasheet suggest: constant 58 used to calculate centimeter distance and 178 is used to calculate inch distance
* * distance (cm) = (Echo Pin HIGH Time uS) / 58 
* * distance (inch) = (Echo Pin HIGH Time uS) / 178

Now, we know that we need to trigger sensor from its TRIG pin and need to read value of ECHO pin. Therefore, we need to allocate 2 GPIO pins from UP2 board to use one as input to read digital value (ECHO) and one as output to send digital signal (TRIG) pin.

I will use PIN 13-15 from UP2 GP-Bus Expansion.

In Linux SYSFS:
- PIN 13 corresponds to GPIO 432 in sysfs
- PIN 15 corresponds to GPIO 431 in sysfs

See UP2 board pinout: https://wiki.up-community.org/Pinout_UP2

## Using SYSFS 

So let's write our code to read distance continuously from HC-SR4 sensor and print the distance in centimeters until you press 'q' or 'Q':

Basic logic of code is :
* Set mode of GPIO pins for TRIG and ECHO pins
* Toggle TRIG pin for 10us
* Measure the us for ECHO PIN get HIGH
* Calculate Distance with formula (duration (uS) / 59.0) in centimeters

Logic is simple but, in order to play with GPIO pins, you need to access devices in Linux. That requires some implementation, and even more for I2C, SPI, PCI-e and so on.
 
We need to set mode of pin (input/output), we need to export pins if not exported into file system, read and set values so in order to do that I wrote 4 methods as shown below:

```
gpio_set_mode
gpio_export
gpio_get_value
gpio_set_value
```
Code can be accessed from : [Ultrasound Distance Measure with UP2 Board](https://github.com/odundar/ultrasound_sensor/blob/master/legacy_sysfs/ultrasound_legacy.c)

Let's have a quick look at the code.

### Code Block:
```
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
```

You can build with below instructions on your UP2 board make sure you installed `build-essentials` package.

### Build Run Instruction:
```
$ cd legacy_sys/
$ gcc ultrasound.c -o Ultrasound
 ```
or
```
$ mkdir build
$ cmake ../
$ make all

$ sudo ./Ultrasound
 ```

## Using MRAA 

What if we wanted to use MRAA?

### Install MRAA and UPM

Let's first install MRAA and UPM into UP2 board Ubuntu 16.04 with following 

See: https://github.com/intel-iot-devkit/mraa 
See: https://iotdk.intel.com/docs/master/upm/installing.html 

```
sudo add-apt-repository ppa:mraa/mraa
sudo apt-get update
sudo apt-get install libmraa1 libmraa-dev libmraa-java python-mraa python3-mraa node-mraa mraa-tools libupm-dev libupm-java python-upm python3-upm node-upm upm-examples
```

There wouldn't be any need for reinventing the wheel, so we can just use the MRAA methods:
```
mraa_gpio_init
mraa_gpio_dir
mraa_gpio_write
mraa_gpio_read
```
to play with GPIO. Need to change to MRAA's own Pin Numbering instead of SYSFS GPIO Numbers.

see : https://github.com/intel-iot-devkit/mraa/blob/master/docs/up2.md

Code can be accessed from : [Ultrasound Distance Measure with UP2 Board - MRAA](https://github.com/odundar/ultrasound_sensor/blob/master/mraa/ultrasound_mraa.c)

### MRAA - Code Block
```
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
```

### Build and Run Instructions

```
$ gcc ultrasound_upm.c -o UltrasoundMRAA -I/usr/include/mraa/ -L/usr/lib/x86_64-linux-gnu/ -lmraa
```
or 
```
$ mkdir build
$ cmake ../
$ make all

$ sudo ./UltrasoundMRAA
 ```

## Using UPM

What if we skip MRAA and just use UPM which already have HC-SR04 sensor in its library?

UPM library already does sensor initialisation so no need to access for GPIO pins when we defined the PIN numbers.

Code can be accessed from : [Ultrasound Distance Measure with UP2 Board - UPM](https://github.com/odundar/ultrasound_sensor/blob/master/mraa/ultrasound_upm.c)

### UPM - Code Block
```
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
```

### Build and Run Instructions
```
$ gcc ultrasound_upm.c -o UltrasoundUPM -I/usr/include/upm/ -L/usr/lib/x86_64-linux-gnu/ -lmraa -lupmc-hcsr04
```
or 
```
$ mkdir build
$ cmake ../
$ make all

$ sudo ./UltrasoundUPM
 ```
