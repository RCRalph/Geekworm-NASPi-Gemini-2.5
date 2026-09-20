CC = gcc
CFLAGS = -Wall -std=gnu2x -O2

all: pwm_fan_control power_control soft_reboot soft_shutdown

pwm_fan_control: pwm_fan_control.c circular_buffer.c
	$(CC) $(CFLAGS) pwm_fan_control.c circular_buffer.c -o pwm_fan_control -lwiringPi

power_control: power_control.c
	$(CC) $(CFLAGS) power_control.c -o power_control -lwiringPi

soft_reboot: soft_reboot.c
	$(CC) $(CFLAGS) soft_reboot.c -o soft_reboot -lwiringPi -D SLEEP_TIME=400000

soft_shutdown: soft_reboot.c
	$(CC) $(CFLAGS) soft_reboot.c -o soft_shutdown -lwiringPi -D SLEEP_TIME=2000000

.PHONY: clean
clean:
	rm pwm_fan_control power_control soft_shutdown soft_reboot
