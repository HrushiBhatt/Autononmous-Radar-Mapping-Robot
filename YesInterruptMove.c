#include "open_interface.h"          // Interface for the Roomba robot
#include "Timer.h"                   // Timer utilities
#include "new_uart_interrupt.h"      // UART interrupt handling
#include "adc.h"                     // ADC sensor readings
#include "cyBot_Scan.h"              // Scanning utilities

volatile char received_char;         // Stores the most recent UART character received

// Function to play a song using the Roomba speaker
void play_music() {
    unsigned char notes[54] =    {79, 76, 72, 76, 79, 84, 88, 86, 84, 76, 78, 79, 79, 79, 88, 86, 84, 83, 81, 83, 84, 84, 79, 76, 72};
    unsigned char duration[54] = {36, 24, 48, 48, 48, 92, 36, 24, 48, 48, 48, 92, 24, 24, 60, 24, 36, 92, 24, 24, 48, 48, 48, 48, 48};
    oi_loadSong(0, 54, notes, duration);
    oi_play_song(0);
}

// Detects if the robot is near a boundary (white tape)
int boundary_detect(oi_t *sensor_data) {
    oi_update(sensor_data);
    return (sensor_data->cliffLeftSignal >= 2600 || sensor_data->cliffRightSignal >= 2600 || 
            sensor_data->cliffFrontLeftSignal >= 2600 || sensor_data->cliffFrontRightSignal >= 2600);
}

// Detects if the robot is near a hole (black area)
int hole_detect(oi_t *sensor_data) {
    oi_update(sensor_data);
    return (sensor_data->cliffLeftSignal <= 1000 || sensor_data->cliffRightSignal <= 1000 || 
            sensor_data->cliffFrontLeftSignal <= 1000 || sensor_data->cliffFrontRightSignal <= 1000);
}

// Detects if the robot has bumped into a tall object (e.g., tree or rock)
int tall_tree_detect(oi_t *sensor_data) {
    oi_update(sensor_data);
    return (sensor_data->bumpLeft || sensor_data->bumpRight);
}

// Main function
int main(void) {
    timer_init();                     // Initialize timers
    uart_interrupt_init();           // Initialize UART with interrupts
    oi_t *sensor_data = oi_alloc();  // Allocate memory for sensor data
    oi_init(sensor_data);            // Initialize Roomba sensors

    lcd_init();                      // Initialize the LCD display

    int boundary, hole, tall_tree;   // Detection flags
    int sum = 0;                     // Total distance moved
    double currentDegree = 90;       // Starting angle
    char sendAngle[100];             // Buffer to send angle and distance data
    int pleaseScan = 1;              // Flag to determine if scanning is needed

    while (1) {
        // Continuously update sensor flags
        boundary = boundary_detect(sensor_data);
        hole = hole_detect(sensor_data);
        tall_tree = tall_tree_detect(sensor_data);

        switch(received_char) {
        case 'q': // Play music
            play_music();
            break;

        case 'w': // Move forward with obstacle checking
            if (boundary || hole || tall_tree) {
                oi_setWheels(0, 0);
                if (boundary) {
                    uart_sendStr("\r\nBoundary detected: ");
                    if (sensor_data->cliffLeftSignal >= 2600) uart_sendStr("[Left] ");
                    if (sensor_data->cliffRightSignal >= 2600) uart_sendStr("[Right] ");
                    if (sensor_data->cliffFrontLeftSignal >= 2600) uart_sendStr("[Front_Left] ");
                    if (sensor_data->cliffFrontRightSignal >= 2600) uart_sendStr("[Front_Right] ");
                } else if (hole) {
                    uart_sendStr("\r\nHole detected: ");
                    if (sensor_data->cliffLeftSignal <= 1000) uart_sendStr("[Left] ");
                    if (sensor_data->cliffRightSignal <= 1000) uart_sendStr("[Right] ");
                    if (sensor_data->cliffFrontLeftSignal <= 1000) uart_sendStr("[Front_Left] ");
                    if (sensor_data->cliffFrontRightSignal <= 1000) uart_sendStr("[Front_Right] ");
                } else if (tall_tree) {
                    uart_sendStr("\r\nRock detected: ");
                    if (sensor_data->bumpLeft) uart_sendStr("[Left] ");
                    if (sensor_data->bumpRight) uart_sendStr("[Right] ");
                }
            } else {
                if (sum < getMaxDistance() && pleaseScan) {
                    oi_setWheels(150, 150);
                    sum += sensor_data->distance;
                    oi_update(sensor_data);
                    lcd_printf("%d", sum);
                    char newThing[3];
                    sprintf(newThing, "%d", sum);
                    uart_sendStr(newThing);
                } else {
                    oi_setWheels(0, 0);
                    pleaseScan = 0;
                    uart_sendStr("Please rescan");
                    sum = 0;
                }
            }
            break;

        case 's': // Move backward slowly
            oi_setWheels(-25, -25);
            break;

        case 'a': // Turn left by 1 degree
            if (currentDegree < 180.0) {
                double angle_turned = 0.0;
                sensor_data->angle = 0;
                oi_setWheels(25, -25);
                while (angle_turned < 1.0) {
                    timer_waitMillis(10);
                    oi_update(sensor_data);
                    angle_turned += fabs(sensor_data->angle);
                }
                oi_setWheels(0, 0);
                currentDegree += 1.0;
                if (currentDegree > 180.0) currentDegree = 180.0;

                double displayedDegree = ((currentDegree - 60.0) * (180.0 / 61.0));
                if (displayedDegree < 0) displayedDegree = 0;
                if (displayedDegree > 180) displayedDegree = 180;

                lcd_printf("Turning Left: %.1lf", displayedDegree);
                sprintf(sendAngle, "Angle: %0.2lf, Distance Moved: %d, Max Distance: %d", displayedDegree, sum, getMaxDistance());
                uart_sendStr(sendAngle);
                uart_sendChar('\n');
                uart_sendChar('\r');
            }
            break;

        case 'd': // Turn right by 1 degree
            if (currentDegree > 0.0) {
                double angle_turned = 0.0;
                sensor_data->angle = 0;
                oi_setWheels(-25, 25);
                while (angle_turned < 1.0) {
                    timer_waitMillis(10);
                    oi_update(sensor_data);
                    angle_turned += fabs(sensor_data->angle);
                }
                oi_setWheels(0, 0);
                currentDegree -= 1.0;
                if (currentDegree < 0.0) currentDegree = 0.0;

                double displayedDegree = ((currentDegree - 60.0) * (180.0 / 61.0));
                if (displayedDegree < 0) displayedDegree = 0;
                if (displayedDegree > 180) displayedDegree = 180;

                lcd_printf("Turning Right: %.1lf", displayedDegree);
                sprintf(sendAngle, "Angle: %0.2lf, Distance Moved: %d, Max Distance: %d", displayedDegree, sum, getMaxDistance());
                uart_sendStr(sendAngle);
                uart_sendChar('\n');
                uart_sendChar('\r');
            }
            break;

        case '1': // Perform short scan
            shortScan();
            break;

        case '2': // Perform long scan
            longScan();
            pleaseScan = 1;
            currentDegree = 90;
            break;

        case '9': // Turn left 90 degrees
            if (currentDegree <= 90) {
                double angle_turned = 0.0;
                sensor_data->angle = 0;
                oi_setWheels(25, -25);
                while (angle_turned < 90.0) {
                    timer_waitMillis(10);
                    oi_update(sensor_data);
                    angle_turned += fabs(sensor_data->angle);
                }
                oi_setWheels(0, 0);
                currentDegree += 90.0;
                if (currentDegree > 180.0) currentDegree = 180.0;
                lcd_printf("Turning Left 90: %.1lf", currentDegree);

                sprintf(sendAngle, "Angle: %0.2lf, Distance Moved: %d, Max Distance: %d", currentDegree, sum, getMaxDistance());
                uart_sendStr(sendAngle);
                uart_sendChar('\n');
                uart_sendChar('\r');
            }
            break;

        case '0': // Turn right 90 degrees
            if (currentDegree >= -180.0) {
                double angle_turned = 0.0;
                sensor_data->angle = 0;
                oi_setWheels(-25, 25);
                while (angle_turned < 90.0) {
                    timer_waitMillis(10);
                    oi_update(sensor_data);
                    angle_turned += fabs(sensor_data->angle);
                }
                oi_setWheels(0, 0);
                currentDegree -= 90.0;
                if (currentDegree < -180.0) currentDegree = 90.0;
                lcd_printf("Turning Right 90: %.1lf", currentDegree);

                sprintf(sendAngle, "Angle: %0.2lf, Distance Moved: %d, Max Distance: %d", currentDegree, sum, getMaxDistance());
                uart_sendStr(sendAngle);
                uart_sendChar('\n');
                uart_sendChar('\r');
            }
            break;

        default: // Stop if unknown key is pressed
            oi_setWheels(0, 0);
            break;
        }

        received_char = 0; // Reset received char for next command
    }
}
