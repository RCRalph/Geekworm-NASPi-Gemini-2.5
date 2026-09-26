#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

#include "circular_buffer.h"

#define PWM_PIN 1
#define PWM_RANGE 1024
#define PWM_MIN (PWM_RANGE / 4)
#define PWM_MAX (PWM_RANGE - 1)

#define TEMP_MIN 50
#define TEMP_MAX 60

static volatile sig_atomic_t keepRunning = 1;

void handleInterrupt(int signalValue) {
    keepRunning = 0;
}

void setupPWM() {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Error setting up GPIO pins\n");
        exit(1);
    }

    pinMode(PWM_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_BAL);
    pwmWrite(PWM_PIN, PWM_RANGE);
}

float getCPUTemperature() {
    FILE* file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (file == NULL) {
        perror("Error opening /sys/class/thermal/thermal_zone0/temp");
        return NAN;
    }

    char buffer[16];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("Error reading CPU temperature");
        fclose(file);
        return NAN;
    }

    float result = atof(buffer) / 1000;
    fclose(file);

    return result;
}

int getFanSpeed(struct CircularBuffer* temperatureBuffer) {
    float maxTemperature = circularBufferMax(temperatureBuffer);

    if (isnan(maxTemperature)) return PWM_MAX;

    if (maxTemperature < TEMP_MIN) {
        maxTemperature = TEMP_MIN;
    } else if (maxTemperature > TEMP_MAX) {
        maxTemperature = TEMP_MAX;
    }

    float fanPercentage = (maxTemperature - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);

    return lround(PWM_MIN + fanPercentage * (PWM_MAX - PWM_MIN));
}

int main(void) {
    signal(SIGINT, handleInterrupt);
    signal(SIGTERM, handleInterrupt);

    setupPWM();

    struct CircularBuffer temperatureBuffer;
    circularBufferInit(&temperatureBuffer);

    int lastFanSpeed = PWM_MAX, newFanSpeed;
    float temperature;

    while (keepRunning) {
        sleep(1);

        temperature = getCPUTemperature();
        if (isnan(temperature)) {
            continue;
        }

        circularBufferAdd(&temperatureBuffer, temperature);
        newFanSpeed = getFanSpeed(&temperatureBuffer);

        if (lastFanSpeed != newFanSpeed) {
            fprintf(
                stderr,
                "Changing fan speed to %d%% (last max temperature: %.1f°C)\n",
                (newFanSpeed * 100) / PWM_MAX,
                temperature);

            pwmWrite(PWM_PIN, newFanSpeed);
            lastFanSpeed = newFanSpeed;
        }
    }

    fprintf(stderr, "Process interrupted\n");
    pwmWrite(PWM_PIN, 0);
    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(PWM_PIN, LOW);

    return 1;
}
