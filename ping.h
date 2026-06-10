/*
 * movement.h
 *
 *  Created on: Nov 21, 2024
 *      Author: somayg
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

double move_forward(oi_t * sensor, double millimeters);

void turn_clockwise(oi_t * sensor, double degrees);

void turn_counterclockwise(oi_t * sensor, double degrees);

void move_backward(oi_t * sensor, double millimeters);

void handleCollisionLeft(oi_t * sensor);

void handleCollisionRight(oi_t * sensor);

void travel(oi_t * sensor, double millimeters);

void putty(char * pointer);



#endif /* MOVEMENT_H_ */
