/*
 * movement.c
 *
 *  Created on: Nov 21, 2024
 *      Author: somayg
 */

#include "open_interface.h"
#include "timer.h"
#include "uart.h"
#include "movement.h"
#include "cliff.h"

void putty(char * pointer);

void move_backward(oi_t * sensor, double millimeters) {
    double distanceTraveled = 0;
    oi_setWheels(-100, -100);

    while (distanceTraveled > -(millimeters)) {
        oi_update(sensor);
        distanceTraveled += sensor->distance;
    }

    oi_setWheels(0,0);
}

void turn_clockwise(oi_t * sensor, double degrees) {
    double degreesTurned = 0;
    oi_setWheels(-50,50);

    while (degreesTurned > - degrees) {
        oi_update(sensor);
        degreesTurned += sensor->angle;
    }

    oi_setWheels(0,0);
}

void turn_counterclockwise(oi_t * sensor, double degrees) {
    double degreesTurned = 0;
    oi_setWheels(50,-50);

    while (degreesTurned < degrees){
        oi_update(sensor);
        degreesTurned += sensor->angle;
    }

    oi_setWheels(0,0);
}

void handleCollisionLeft(oi_t * sensor) {
    putty("Collision - LEFT\n");
    double distanceTraveled = 0;
    timer_waitMillis(500);
    move_backward(sensor, 150);
    timer_waitMillis(500);
    turn_clockwise(sensor, 90);
    timer_waitMillis(500);
    while(distanceTraveled < 200) {
        oi_setWheels(100,100);
        oi_update(sensor);
        distanceTraveled += sensor->distance;
    }
    oi_setWheels(0,0);
    timer_waitMillis(500);
    turn_counterclockwise(sensor,90);
    timer_waitMillis(500);
}

void handleCollisionRight(oi_t * sensor) {
    putty("Collision - RIGHT\n");
    double distanceTraveled = 0;
    timer_waitMillis(500);
    move_backward(sensor, 150);
    timer_waitMillis(500);
    turn_counterclockwise(sensor,90);
    timer_waitMillis(500);
    while(distanceTraveled < 200) {
        oi_setWheels(100,100);
        oi_update(sensor);
        distanceTraveled += sensor->distance;
    }
    oi_setWheels(0,0);
    timer_waitMillis(500);
    turn_clockwise(sensor, 90);
    timer_waitMillis(500);
}

double move_forward(oi_t * sensor, double millimeters){
    double distanceTraveled = 0;
    oi_setWheels(100, 100);

    while (distanceTraveled < millimeters) {
        oi_update(sensor);
        int flag = scan_for_cliff(sensor);
        if (sensor->bumpLeft) {
            oi_setWheels(0,0);
            handleCollisionLeft(sensor);
            distanceTraveled = millimeters;
            //oi_setWheels(100,100);
        }
        if (sensor->bumpRight) {
            oi_setWheels(0,0);
            handleCollisionRight(sensor);
            distanceTraveled = millimeters;
            //oi_setWheels(100,100);
        }

        if(flag > 0){
          oi_setWheels(0, 0);
          move_for_flag(sensor, scan_for_cliff(sensor));
          oi_setWheels(100,100);
        }

        distanceTraveled += sensor->distance;

    }

    oi_setWheels(0,0);

    return distanceTraveled;
}


void travel(oi_t * sensor, double millimeters) {
    double distanceTraveled = 0;

    while (distanceTraveled < (millimeters-50)) {
        double distance = move_forward(sensor, millimeters-distanceTraveled);
        distanceTraveled += distance;


    }
}

void putty(char * pointer) {
    while (*(pointer)) {
        uart_sendChar(*pointer);
        pointer++;
    }
}




