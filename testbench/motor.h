#ifndef MOTOR_H
# define MOTOR_H

// values depending on the motor supply voltage:
#define T_MAX       400 // max time (-45° -> +45°)
#define V_MAX       225 // max velocity
#define ANGLE_STEP  1   // min angle between 2 steps

#define MAX_ANGLE 40
#define MIN_ANGLE -40

void vMotorInit();
void prvMotorAngle2PWM(int degres);
void vMotorSetPosition(int a, int v);
void vMotorTask();

#endif

