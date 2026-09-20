#include <linux/reboot.h>
#include <signal.h>
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/time.h>
#include <unistd.h>
#include <wiringPi.h>

#define SHUTDOWN_PIN 7
#define BOOT_PIN 0

#define US_IN_SECOND 1000000  // `US` = microseconds
#define PULSE_TIMEOUT US_IN_SECOND / 50
#define REBOOT_PULSE_MIN_DURATION US_IN_SECOND * 2 / 10  // 200 ms
#define REBOOT_PULSE_MAX_DURATION US_IN_SECOND * 6 / 10  // 600 ms

static volatile sig_atomic_t keepRunning = 1;

void handleInterrupt(int signalValue) {
    keepRunning = 0;
}

time_t getTimeDifferenceUS(struct timeval* start, struct timeval* end) {
    return (end->tv_sec - start->tv_sec) * US_IN_SECOND + (end->tv_usec - start->tv_usec);
}

int getRebootMethod() {
    struct timeval start, current;
    gettimeofday(&start, NULL);

    do {
        gettimeofday(&current, NULL);

        if (getTimeDifferenceUS(&start, &current) > REBOOT_PULSE_MAX_DURATION) {
            return RB_POWER_OFF;
        }

        usleep(PULSE_TIMEOUT);
    } while (digitalRead(SHUTDOWN_PIN) == HIGH);

    gettimeofday(&current, NULL);
    if (getTimeDifferenceUS(&start, &current) > REBOOT_PULSE_MIN_DURATION) {
        return RB_AUTOBOOT;
    }

    return 0;
}

int main() {
    signal(SIGINT, handleInterrupt);
    signal(SIGTERM, handleInterrupt);

    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Error setting up GPIO pins\n");
        return 1;
    }

    pinMode(SHUTDOWN_PIN, INPUT);
    pinMode(BOOT_PIN, OUTPUT);
    digitalWrite(BOOT_PIN, HIGH);

    int rebootMethod;
    while (keepRunning) {
        usleep(PULSE_TIMEOUT);

        if (digitalRead(SHUTDOWN_PIN) == LOW) {
            continue;
        }

        rebootMethod = getRebootMethod();
        switch (rebootMethod) {
            case RB_POWER_OFF:
                printf("Powering off...\n");
                break;
            case RB_AUTOBOOT:
                printf("Rebooting...\n");
                break;
            default:
                continue;
        }

        reboot(rebootMethod);
        return 0;
    }

    fprintf(stderr, "Process interrupted\n");
    pinMode(SHUTDOWN_PIN, INPUT);
    pinMode(BOOT_PIN, INPUT);

    return 2;
}