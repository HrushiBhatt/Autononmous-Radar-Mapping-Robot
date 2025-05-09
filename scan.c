#include "cyBot_Scan.h"
#include <stdio.h>
#include "new_uart_interrupt.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "adc.h"

int maxDistance = 70; // Default max distance in cm

// Sets the max distance the robot can move forward
void setMaxDistance(int maxDist) {
    maxDistance = maxDist;
}

// Returns the max allowed distance
int getMaxDistance() {
    return maxDistance;
}

// Converts IR ADC value to distance in cm
float IR_to_cm(uint16_t irVal) {
    if(irVal <= 450) {
        return 9999.0f; // Invalid reading or no object
    }
    float average = (float)irVal;
    return 120000.0f * powf(average, -1.23f); // Empirical formula
}

// Structure to store object detection data
typedef struct {
    int minDegree;
    int maxDegree;
    float Irsensor;
    int midPoint;
    float objectWidth;
} ObjectData;

ObjectData *detectedObject = NULL;

// Performs a short IR scan from 70 to 110 degrees
void shortScan() {
    cyBOT_init_Scan(0b0101);
    adc_init();

    right_calibration_value = 274750;
    left_calibration_value = 1214500;

    cyBOT_Scan_t scan;
    int IRValue, degree, objectCount = 0, inObject = 0, temp, minIndex = 0;
    int IRscan[3], IRtotal;
    float currentMinIR = 9999.0;

    for(degree = 70; degree <= 110; degree++) {
        cyBOT_Scan(degree, &scan);
        IRscan[0] = adc_read();
        IRscan[1] = adc_read();
        IRscan[2] = adc_read();

        IRtotal = IRscan[0] + IRscan[1] + IRscan[2];
        IRValue = IRtotal / 3;
        float IR_cm = IR_to_cm(IRValue);

        if(IR_cm > 0 && IR_cm <= 70) {
            uart_sendChar('1'); // Object detected

            if (!inObject) {
                currentMinIR = 9999;
                detectedObject = realloc(detectedObject, (objectCount + 1) * sizeof(*detectedObject));
                detectedObject[objectCount].minDegree = degree;
                detectedObject[objectCount].maxDegree = degree;
                inObject = 1;
                objectCount++;
            } else {
                detectedObject[objectCount - 1].maxDegree = degree;
            }

            if (IR_cm < currentMinIR) currentMinIR = IR_cm;
        } else {
            uart_sendChar('0'); // No object
            inObject = 0;

            if(objectCount > 0) {
                int i = objectCount - 1;
                temp = detectedObject[i].maxDegree - detectedObject[i].minDegree;
                detectedObject[i].Irsensor = currentMinIR;
                detectedObject[i].objectWidth = calculateLinearWidth(currentMinIR, temp);
                detectedObject[i].midPoint = (detectedObject[i].minDegree + detectedObject[i].maxDegree) / 2;
            }
        }

        for(int i = 0; i < objectCount; i++) {
            if(detectedObject[i].objectWidth < detectedObject[minIndex].objectWidth) {
                minIndex = i;
            }
        }
    }

    if (inObject && objectCount > 0) {
        int i = objectCount - 1;
        temp = detectedObject[i].maxDegree - detectedObject[i].minDegree;
        detectedObject[i].Irsensor = currentMinIR;
        detectedObject[i].objectWidth = (currentMinIR < 9999 && currentMinIR > 0) ? calculateLinearWidth(currentMinIR, temp) : 0;
        detectedObject[i].midPoint = (detectedObject[i].minDegree + detectedObject[i].maxDegree) / 2;
    }

    uart_sendChar('\n');
    uart_sendChar('\r');

    char Buffer[200];
    int b, min = 9999;

    for(b = 0; b < objectCount; b++) {
        sprintf(Buffer, "Min Degree: %d MaxDegree: %d, Irsensor: %f, Midpoint: %d, ObjectWidth: %f",
                detectedObject[b].minDegree, detectedObject[b].maxDegree, detectedObject[b].Irsensor,
                detectedObject[b].midPoint, detectedObject[b].objectWidth);
        uart_sendStr(Buffer);
        uart_sendChar('\n');
        uart_sendChar('\r');

        if(detectedObject[b].Irsensor < min && objectCount != 0)
            setMaxDistance(detectedObject[b].Irsensor);

        if(detectedObject[b].objectWidth >= 10 && detectedObject[b].objectWidth < 15) {
            char FoundObject[20];
            sprintf(FoundObject, "Found! Angle: %d", detectedObject[b].midPoint);
            uart_sendStr(FoundObject);
        }
    }

    if(objectCount == 0) {
        uart_sendStr("Clear to go straight min distance");
        setMaxDistance(70);
    } else {
        uart_sendStr("Cannot go straight object in path");
    }

    uart_sendChar('\n');
    uart_sendChar('\r');
}

// Performs a wide scan from 1 to 180 degrees
void longScan() {
    cyBOT_init_Scan(0b0101);
    adc_init();

    right_calibration_value = 274750;
    left_calibration_value = 1214500;

    cyBOT_Scan_t scan;
    int IRValue, degree, objectCount = 0, inObject = 0, temp, minIndex = 0;
    int IRscan[3], IRtotal;
    float currentMinIR = 9999.0;

    for(degree = 1; degree <= 180; degree += 3) {
        cyBOT_Scan(degree, &scan);
        IRscan[0] = adc_read();
        IRscan[1] = adc_read();
        IRscan[2] = adc_read();

        IRtotal = IRscan[0] + IRscan[1] + IRscan[2];
        IRValue = IRtotal / 3;
        float IR_cm = IR_to_cm(IRValue);

        if(IR_cm > 0 && IR_cm <= 70) {
            uart_sendChar('1');

            if (!inObject) {
                currentMinIR = 9999;
                detectedObject = realloc(detectedObject, (objectCount + 1) * sizeof(*detectedObject));
                detectedObject[objectCount].minDegree = degree;
                detectedObject[objectCount].maxDegree = degree;
                inObject = 1;
                objectCount++;
            } else {
                detectedObject[objectCount - 1].maxDegree = degree;
            }

            if (IR_cm < currentMinIR) currentMinIR = IR_cm;
        } else {
            uart_sendChar('0');
            inObject = 0;

            if(objectCount > 0) {
                int i = objectCount - 1;
                temp = detectedObject[i].maxDegree - detectedObject[i].minDegree;
                detectedObject[i].Irsensor = currentMinIR;
                detectedObject[i].objectWidth = calculateLinearWidth(currentMinIR, temp);
                detectedObject[i].midPoint = (detectedObject[i].minDegree + detectedObject[i].maxDegree) / 2;
            }
        }

        for(int i = 0; i < objectCount; i++) {
            if(detectedObject[i].objectWidth < detectedObject[minIndex].objectWidth)
                minIndex = i;
        }
    }

    if (inObject && objectCount > 0) {
        int i = objectCount - 1;
        temp = detectedObject[i].maxDegree - detectedObject[i].minDegree;
        detectedObject[i].Irsensor = currentMinIR;
        detectedObject[i].objectWidth = (currentMinIR < 9999 && currentMinIR > 0) ? calculateLinearWidth(currentMinIR, temp) : 0;
        detectedObject[i].midPoint = (detectedObject[i].minDegree + detectedObject[i].maxDegree) / 2;
    }

    uart_sendChar('\n');
    uart_sendChar('\r');

    char Buffer[200];
    int b, min = 9999;

    for(b = 0; b < objectCount; b++) {
        sprintf(Buffer, "Min Degree: %d MaxDegree: %d, Irsensor: %f, Midpoint: %d, ObjectWidth: %f",
                detectedObject[b].minDegree, detectedObject[b].maxDegree, detectedObject[b].Irsensor,
                detectedObject[b].midPoint, detectedObject[b].objectWidth);

        uart_sendStr(Buffer);
        uart_sendChar('\n');
        uart_sendChar('\r');

        if(detectedObject[b].Irsensor < min && objectCount != 0)
            setMaxDistance(detectedObject[b].Irsensor);

        if(detectedObject[b].objectWidth >= 9.5) {
            char FoundObject[20];
            sprintf(FoundObject, "Found! Angle: %d", detectedObject[b].midPoint);
            uart_sendStr(FoundObject);
        }

        if(objectCount == 0) setMaxDistance(70);
    }
}

// Stores and returns the most recent Ping (not used here, but may be for future PING sensor logic)
float returnPing;
void setDistance(float Ping) { returnPing = Ping; }
float getPing() { return returnPing; }

// Stores and returns the midpoint of the final target object
int ReturnMidpoint;
void setMidpoint(int midPoint) { ReturnMidpoint = midPoint; }
int getMidPoint() { return ReturnMidpoint; }

// Converts IR ADC value to distance in meters using voltage
int IrToMeters(int adc_value) {
    float voltage = (adc_value * 3.3) / 4090.2;
    if(voltage <= 0.42) return -1;
    float distance = 27.86 / (voltage - 0.42);
    return distance;
}

// Calculates the linear width of an object given
