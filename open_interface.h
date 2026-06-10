

/**
 * main.c
 */
#include "lcd.h"
#include "open_interface.h"
#include "movement.h"
#include "open_interface.h"
#include "timer.h"
#include "uart.h"
#include <stdbool.h>
#include "servo.h"
#include "adc.h"
#include "ping.h"
//#include "cyBot_Scan.h"


struct scanVal sensor_data_array[91];
struct object objs[20];
int objCounter = 0;
float destDistance = 0;
int destCenter = 0;

struct scanVal scanAngle(int angle);
void scan_180();
int findObjects();
int checkPath();

struct object {
    float pingDist;
    float irDist;
    int startDegree;
    int endDegree;
    int center;
    int width;
    float linWidth;
};

struct scanVal {
    float IR;
    float Ping;
};

void main()
{
	//our task is to move our cybot autonomously, moving bit by bit scanning and checking for objects

    //step 1 initializations:
    lcd_init();
    timer_init();
    uart_init(115200);
    adc_init();
    ping_init();
    servo_init();
    //step 2 necessary variables:
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    int isStart = 1;
    int atDest = 1;
    char buffer[50];
//    cyBOT_Scan_t scan;


    //step3 autonomous movement + object detection
//
    while(isStart){
        int turnPoint;
        int turnMag;

      //scan for object
        oi_setWheels(0,0);
        scan_180();

        //check object widths
        isStart = findObjects();
        lcd_printf(isStart);
        if (isStart) {
            turnPoint = checkPath();
            sprintf(buffer, "turn point is %d\n", turnPoint);
            putty(buffer);
            if (turnPoint >= 150) { //condition met, bot moves forward and scans but stops
                move_forward(sensor_data, 300);
            } else {
                turnMag = 155 - turnPoint; //FIXME: change?
                turn_clockwise(sensor_data, turnMag);
            }
        } else { //final destination found
            
            //move towards the object
            if(destCenter < 90){
                turn_clockwise(sensor_data, 90 - destCenter);
            }
            else{
                turn_counterclockwise(sensor_data, destCenter - 90);
            }
            move_forward(sensor_data, (destDistance * 10) - 70.0);
            playSong(0); //play sound to alert that we've reached final destination
            lcd_printf("Your food has arrived.");
        }
    }

    //while (atDest) {

    //}



}

 //this function when called in main plays the short sound
int playSong(int songIndex){
       unsigned char notes [15] = {64,70,64,70,64,70,64,70};
       unsigned char duration[15] = {8,8,8,8,8,8,8,8};
        oi_loadSong(songIndex, 8, notes, duration);
        oi_play_song(songIndex);

        return 0;
}

//function checks the 120 degree cone in front of the bot 
int checkPath() {
    int i;
    int inPath[10];
    int inPathCounter = 0;
    int mostRight = 155;//FIXME: change
    char buffer[50];
    for (i = 0; i < objCounter; i++) {
        if (objs[i].center > 30 && objs[i].center < 150 && objs[i].pingDist < 40) {
            inPath[inPathCounter] = i;
            inPathCounter++;
            if (objs[i].center < mostRight) {
                mostRight = objs[i].center;
            }
        }
    }
    sprintf(buffer, "most right is %d \n", mostRight);
    putty(buffer);

    return mostRight;
}

//scan distance using ir and ping
struct scanVal scanAngle(int angle) {
    servo_move(angle);
    struct scanVal distVals;
    int i;
    int IRVal = 0;
    float PingVal[3];
    for(i = 0; i < 3; i++) {
        IRVal += adc_read();
        PingVal[i] = ping_read();
    }
    distVals.IR = (float)(IRVal/3);

    if (PingVal[0] > PingVal[1]){
        if (PingVal[0] > PingVal[2]) {
            if (PingVal[1] > PingVal[2]) {
                distVals.Ping = PingVal[1];
            } else {
                distVals.Ping = PingVal[2];
            }
        } else {
            distVals.Ping = PingVal[0];
        }
    } else {
        if (PingVal[2] > PingVal[0]) {
            if (PingVal[1] > PingVal[2]) {
                distVals.Ping = PingVal[2];
            } else {
                distVals.Ping = PingVal[1];
            }
        } else {
            distVals.Ping = PingVal[0];
        }
    }

    distVals.Ping = ((((distVals.Ping) / 16000000) * 34300) / 2);
    char buffer[50];
    sprintf(buffer, "%.2lf %.2lf \n", distVals.IR, distVals.Ping);

    putty(buffer);
    lcd_printf(buffer);

    return distVals;
}

//scans 180 with threshold distance of 65
void scan_180() {

    struct scanVal valDist;

    //putty("Angle(degrees)\tPING(cm)\tIR(cm)\n");
    int j;
    for (j = 0; j < 181; j += 2) {
        valDist = scanAngle(j);

        if (valDist.IR > 65.0) {
            valDist.IR = 65.0;
        }

        if (valDist.Ping > 65.0) {
            valDist.Ping = 65.0;
        }

        sensor_data_array[j/2] = valDist;

        /*char buffer[50];
        float bufferFloat = (valDist.Ping)-5;
        sprintf(buffer, "%d\t\t%.1f\t\t%.1f\n", j, bufferFloat, valDist.IR);
        putty(buffer);*/
        timer_waitMillis(50);
    }
}

//takes data from scan and finds the objects in the path
//returns if destination is found or not by returning 0 or 1 
int findObjects() {

    int newObj = 0;
    int j;
    char buffer[50];
    struct scanVal scan;
    int destNotFound = 1;

    objCounter = 0;

    float lastDist = sensor_data_array[0].IR;
    for (j = 0; j < 91; j++) {
        if (lastDist - sensor_data_array[j].IR > 7 && newObj == 0) {
            objs[objCounter].startDegree = j*2;
            objCounter += 1;
            newObj = 1;
        }
        if (lastDist - sensor_data_array[j].IR < -7 && newObj == 1) {
            objs[objCounter-1].endDegree = j*2;
            newObj = 0;
            if (objs[objCounter-1].endDegree - objs[objCounter-1].startDegree < 5) {
                objCounter -= 1;
            }
        } else if (newObj == 1 && j == 90) {
            objs[objCounter-1].endDegree = j*2;
            newObj = 0;
            if (objs[objCounter-1].endDegree - objs[objCounter-1].startDegree < 5) {
                objCounter -= 1;
            }
        }
        lastDist = sensor_data_array[j].IR;
    }

    putty("Obj #\tAngle\tDistanc\tWidth\tStart\tEnd\tLinear Width\n");

    for (j = 0; j < objCounter; j++) {
        objs[j].width = (objs[j].endDegree - objs[j].startDegree);
        int center = objs[j].endDegree - (objs[j].width/2);
        objs[j].center = center;

        objs[j].pingDist = sensor_data_array[center/2].Ping;
        //objs[j].irDist = scanVal.IR_raw_val;

        float radWidth = ((objs[j].width)*(3.14159))/180;
        objs[j].linWidth = sin(radWidth/2) * objs[j].pingDist * 2;



        float linWidth = objs[j].linWidth;
        sprintf(buffer, "%.2lf", linWidth);
        putty(buffer);
        if (linWidth > 4.0 && linWidth < 8) {
            objs[j].linWidth = 6.0;
        } else if (linWidth > 8 && linWidth < 14.5) {
            objs[j].linWidth = 11.5;
        } else if(linWidth > 14.5 && linWidth < 20.0) {
            objs[j].linWidth = 17.0;
        }

        if (objs[j].linWidth == 6.0 && objs[j].endDegree < 180) {
            destNotFound = 0;
            destCenter = objs[j].center;
            destDistance = objs[j].pingDist;
        }

        sprintf(buffer, "%d\t%d\t%.2f\t%d\t%d\t%d\t%.2f\n", j+1, center, objs[j].pingDist, objs[j].width, objs[j].startDegree, objs[j].endDegree, objs[j].linWidth);
        putty(buffer);
    }

    //cyBOT_Scan((objs[smallObj].endDegree)-((objs[smallObj].width)/2), &scanVal);

    return destNotFound;
}
