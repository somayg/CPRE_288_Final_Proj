/*
 * cliff.c
 *
 *  Created on: Dec 11, 2024
 *      Author: somayg
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>

#include "Timer.h"
#include "lcd.h"
#include "open_interface.h"
#include "uart.h"
#include "movement.h"

#include "driverlib/interrupt.h"

bool destination = false;

/**
 * @author Miles Nichols
 * File to detect the tape and felt with the bottom IR sensors
 *
 * Cliff Sensors: The iRobot Create has four cliff sensors, which are used to detect when the robot is about to fall off a ledge or step.
 * These are referred to as: cliffLeft, cliffFrontLeft, cliffFrontRight, and cliffRight
 */

/*
 * function to move the bottom away from the obstacle.
 */
void move_backward_until(oi_t *oi){
    oi_setWheels(10, 10);
    while(!((oi->cliffLeftSignal > 1000 && oi->cliffLeftSignal < 2600) || (oi->cliffFrontLeftSignal > 1000 && oi->cliffFrontLeftSignal < 2600) || (oi->cliffFrontRightSignal > 1000 && oi->cliffFrontRightSignal < 2600) || (oi->cliffRightSignal > 1000 && oi->cliffRightSignal < 2600))){
        oi_update(oi);
    }
    oi_setWheels(0, 0);
}

/*
 * function to move the bot onto the tin foil
 */
void move_to_destination(oi_t *oi){ //move until all sensors and then until the bot is completely on the foil
    oi_setWheels(10, 10);
       while(!((oi->cliffLeftSignal > 1000 && oi->cliffLeftSignal < 2600) && (oi->cliffFrontLeftSignal > 1000 && oi->cliffFrontLeftSignal < 2600) && (oi->cliffFrontRightSignal > 1000 && oi->cliffFrontRightSignal < 2600) && (oi->cliffRightSignal > 1000 && oi->cliffRightSignal < 2600))){
           oi_update(oi);
       }
       oi_setWheels(0, 0);
       move_forward(oi, 300);
       destination = true;
       lcd_printf("FOUND HOME!");
}


/*
 * turns and moves the bot in the correct direction based on the situation.
 */
void move_for_flag(oi_t *oi, int flag){

    //Checks for Hole
    if(flag == 1){
        if (oi->cliffLeftSignal < 200 && oi->cliffFrontLeftSignal > 200 && oi->cliffFrontRightSignal > 200 && oi->cliffRightSignal > 200){ //left
            move_backward_until(oi);
            turn_clockwise(oi, 25);
        }

        if (oi->cliffLeftSignal < 200 && oi->cliffFrontLeftSignal < 200 && oi->cliffFrontRightSignal > 200 && oi->cliffRightSignal > 200){ // left and front left
            move_backward_until(oi);
            turn_clockwise(oi, 45);
        }

        if (oi->cliffLeftSignal > 200 && oi->cliffFrontLeftSignal < 200 && oi->cliffFrontRightSignal > 200 && oi->cliffRightSignal > 200){ //front left
            move_backward_until(oi);
            turn_clockwise(oi, 75);
        }

        if (oi->cliffLeftSignal > 200 && oi->cliffFrontLeftSignal > 200 && oi->cliffFrontRightSignal > 200 && oi->cliffRightSignal < 200){ // right
            move_backward_until(oi);
            turn_counterclockwise(oi, 25);
        }

        if (oi->cliffLeftSignal > 200 && oi->cliffFrontLeftSignal > 200 && oi->cliffFrontRightSignal < 200 && oi->cliffRightSignal < 200){ // right and front right
            move_backward_until(oi);
            turn_counterclockwise(oi, 45);
        }

        if (oi->cliffLeftSignal > 200 && oi->cliffFrontLeftSignal > 200 && oi->cliffFrontRightSignal < 200 && oi->cliffRightSignal > 200){ // front right
            move_backward_until(oi);
            turn_counterclockwise(oi, 75);
        }

        if (oi->cliffLeftSignal > 200 && oi->cliffFrontLeftSignal < 200 && oi->cliffFrontRightSignal < 200 && oi->cliffRightSignal > 200){ //front right and front left
            move_backward_until(oi);
            turn_counterclockwise(oi, 90);
        }

        if(oi->cliffLeftSignal < 200 && oi->cliffFrontLeftSignal < 200 && oi->cliffFrontRightSignal < 200 && oi->cliffRightSignal > 200){ // left, front left, and front right
            move_backward_until(oi);
            turn_clockwise(oi, 80);
        }

        if(oi->cliffLeftSignal > 200 && oi->cliffFrontLeftSignal < 200 && oi->cliffFrontRightSignal < 200 && oi->cliffRightSignal < 200){ // right, front left, and front right
            move_backward_until(oi);
            turn_counterclockwise(oi, 80);
        }

        if(oi->cliffLeftSignal < 200 && oi->cliffFrontLeftSignal < 200 && oi->cliffFrontRightSignal < 200 && oi->cliffRightSignal < 200){ // all
            move_backward_until(oi);
            turn_counterclockwise(oi, 90);
        }
    }

    // Checks for bounds
    if(flag == 2){
        if((oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && !(oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && !(oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && !(oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // left
            move_backward_until(oi);
            turn_clockwise(oi, 25);
        }

        if((oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && !(oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && !(oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // left and left front
            move_backward_until(oi);
            turn_clockwise(oi, 45);
        }

        if(!(oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && !(oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && !(oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // left front
            move_backward_until(oi);
            turn_clockwise(oi, 75);
        }

        if(!(oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && !(oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && !(oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && (oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // right
            move_backward_until(oi);
            turn_counterclockwise(oi, 25);
        }

        if(!(oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && !(oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && (oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // right and right front
            move_backward_until(oi);
            turn_counterclockwise(oi, 45);
        }

        if(!(oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && !(oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                     && (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && !(oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // right front
                 move_backward_until(oi);
                 turn_counterclockwise(oi, 80);
             }

        if(!(oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && !(oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ //right front and left front
            move_backward_until(oi);
            turn_counterclockwise(oi, 75);
        }

        if((oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && !(oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // left left front and right front
            move_backward_until(oi);
            turn_counterclockwise(oi, 90);
        }

        if(!(oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && (oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // right right front and right
            move_backward_until(oi);
            turn_clockwise(oi, 80);
        }

        if((oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) && (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900)
                && (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) && (oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ // all
            move_backward_until(oi);
            turn_counterclockwise(oi, 90);
        }
    }

    //if foil
    /*if(flag == 3){
        if (oi->cliffLeftSignal > 3000 && oi->cliffFrontLeftSignal < 3000 && oi->cliffFrontRightSignal < 3000 && oi->cliffRightSignal < 3000){ //left
                 turn_counterclockwise(oi, 90);
                 move_to_destination(oi);
             }

             if (oi->cliffLeftSignal > 2900 && oi->cliffFrontLeftSignal > 2900 && oi->cliffFrontRightSignal < 2900 && oi->cliffRightSignal < 2900){ // left and front left
                 turn_counterclockwise(oi, 45);
                 move_to_destination(oi);
             }

             if (oi->cliffLeftSignal < 2900 && oi->cliffFrontLeftSignal > 2900 && oi->cliffFrontRightSignal < 2900 && oi->cliffRightSignal < 2900){ //front left
                 turn_counterclockwise(oi, 15);
                 move_to_destination(oi);
             }

             if (oi->cliffLeftSignal < 2900 && oi->cliffFrontLeftSignal < 2900 && oi->cliffFrontRightSignal < 2900 && oi->cliffRightSignal > 2900){ // right
                 turn_clockwise(oi, 90);
                 move_to_destination(oi);
             }

             if (oi->cliffLeftSignal < 2900 && oi->cliffFrontLeftSignal < 2900 && oi->cliffFrontRightSignal > 2900 && oi->cliffRightSignal > 2900){ // right and front right
                 turn_clockwise(oi, 45);
                 move_to_destination(oi);
             }

             if (oi->cliffLeftSignal < 2900 && oi->cliffFrontLeftSignal < 2900 && oi->cliffFrontRightSignal > 2900 && oi->cliffRightSignal < 2900){ // front right
                 turn_clockwise(oi, 15);
                 move_to_destination(oi);
             }

             if (oi->cliffLeftSignal < 2900 && oi->cliffFrontLeftSignal > 2900 && oi->cliffFrontRightSignal > 2900 && oi->cliffRightSignal < 2900){ //front right and front left
                 move_to_destination(oi);
             }

             if(oi->cliffLeftSignal > 2900 && oi->cliffFrontLeftSignal > 2900 && oi->cliffFrontRightSignal > 2900 && oi->cliffRightSignal < 2900){ // left, front left, and front right
                 turn_counterclockwise(oi, 45);
                 move_to_destination(oi);
             }

             if(oi->cliffLeftSignal < 2900 && oi->cliffFrontLeftSignal > 2900 && oi->cliffFrontRightSignal > 2900 && oi->cliffRightSignal > 2900){ // right, front left, and front right
                 turn_clockwise(oi, 45);
                 move_to_destination(oi);
             }

             if(oi->cliffLeftSignal > 2900 && oi->cliffFrontLeftSignal > 2900 && oi->cliffFrontRightSignal > 2900 && oi->cliffRightSignal > 2900){ // all
                 move_to_destination(oi);
             }
        }*/

}

/*
 * returns the flag value to check for holes, bounds, and destination. 1 for hole, 2 for bounds, and 3 for foil
 */
int scan_for_cliff(oi_t *oi){

    int flag = 0;
    //oi_update(oi);
    //lcd_printf("Cliff L: %d\nCliff FL: %d\nCliff FR: %d\nCliff R: %d\n", oi->cliffLeftSignal, oi->cliffFrontLeftSignal, oi->cliffFrontRightSignal, oi->cliffRightSignal);

    if(oi->cliffLeftSignal < 200 || oi->cliffFrontLeftSignal < 200 || oi->cliffFrontRightSignal < 200 || oi->cliffRightSignal < 200){ //hole
        flag = 1;
    }

    if((oi->cliffLeftSignal > 2700 && oi->cliffLeftSignal < 2900) || (oi->cliffFrontLeftSignal > 2700 && oi->cliffFrontLeftSignal < 2900) || (oi->cliffFrontRightSignal > 2700 && oi->cliffFrontRightSignal < 2900) || (oi->cliffRightSignal > 2700 && oi->cliffRightSignal < 2900)){ //bounds
            flag = 2;
    }

    /*if(oi->cliffLeftSignal > 2900 || oi->cliffFrontLeftSignal > 2900 || oi->cliffFrontRightSignal > 2900 || oi->cliffRightSignal > 2900){ //foil
            flag = 3;
    }*/
    return flag;
}



/*
 * main to test cliff functions
 */
//void main(){
//
//    timer_init();
//    lcd_init();
//    oi_t *oi = oi_alloc();
//    oi_init(oi);
//
//    destination = false;
//
//    while(1){
//        oi_update(oi);
//
//        if(scan_for_cliff(oi) != 0){
//            oi_setWheels(0, 0);
//            move_for_flag(oi, scan_for_cliff(oi));
//        }
//        if(!destination){
//            oi_setWheels(50, 50);
//        }
//    }
//    oi_free(oi);
//}

// token : glpat-hxM2TM-TxMy7xKSJSLya





