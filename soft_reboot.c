#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

#define BUTTON_PIN 2

#ifndef SLEEP_DELAY
#define SLEEP_DELAY 0
#endif

int main() {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Error setting up GPIO pins\n");
        return 1;
    }

    pinMode(BUTTON_PIN, OUTPUT);

    // Simulate button press for SLEEP_DELAY seconds
    digitalWrite(BUTTON_PIN, HIGH);
    usleep(SLEEP_DELAY);
    digitalWrite(BUTTON_PIN, LOW);

    return 0;
}