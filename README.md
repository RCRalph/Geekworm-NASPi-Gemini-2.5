# Geekworm NASPi Gemini 2.5
A collection of programs and services, which support daily usage of [Geekworm NASPi Gemini 2.5](https://wiki.geekworm.com/NASPi_Gemini_2.5) - Raspberry Pi 4B NAS server.

This repository was inspired by https://github.com/geekworm-com/xscript, which contains shell and Python versions of the fan and power control scripts. My idea was to rewrite these scripts in C in order to reduce CPU usage, while at the same time maintain control over the fan speeds for various CPU temperatures.

I also found a way to greatly reduce the included fan's loudness and irritating sound frequency changes compared to the Python version of the script. Previously the fan was very noticeable and it was very hard to sleep in the same room as the server. Thinking it was the fan, I thought I should upgrade to a Noctua fan for more silent operation, but after using the scripts defined here I can barely hear the fan even at night. I suspect it might be due to using hardware vs software PWM switching, but I haven't dug deeper into it so I'm not fully sure about it.

## Getting Started
To build the executables, you need to complete the following setup tasks. This list assumes the Raspberry Pi is running a distribution from the Debian family, the instructions may differ for other distributions.
1. **Root permissions**  
    Running the fan and power control services requires root permissions. To install these scripts on your Raspberry Pi, please use a shell with root permissions. If you know the root user's password, you can use:
    ```sh
    su
    ```
    If your account is added to the sudoers group, you can use:
    ```sh
    sudo su
    ```

1. **Install required packages**  
    To build the executables from the source code requires the `wiringpi` library. Its source code and installation steps can be found on [GitHub](https://github.com/wiringpi/wiringpi). Installation steps for pre-build versions are described [here](https://github.com/wiringpi/wiringpi#prebuilt-binaries).

1. **Install build tools**  
    To compile the source code, please install the `build-essential` package:
    ```sh
    apt install build-essential
    ```

1. **Clone the repository**  
    The preferred location of the repository is the `/opt` directory, please navigate to it using `cd` and clone the repository:
    ```sh
    cd /opt
    git clone https://github.com/RCRalph/Geekworm-NASPi-Gemini-2.5
    ```

After all above tasks have been completed, please navigate to the source code:
```sh
cd /opt/Geekworm-NASPi-Gemini-2.5
```

Compile all programs using the following command:
```sh
make all
```

Link the `systemctl` service files, so the executables start their operation at boot time:
```sh
ln -s /opt/Geekworm-NASPi-Gemini-2.5/services/power-control.service /etc/systemd/system/power-control.service
ln -s /opt/Geekworm-NASPi-Gemini-2.5/services/pwm-fan-control.service /etc/systemd/system/pwm-fan-control.service
```

Reload the `systemd` configuration:
```sh
systemctl daemon-reload
```

Check if the services are running, if not use the `systemctl start <service-id>` command:
```sh
systemctl status power-control
systemctl status pwm-fan-control
```

In case of any errors, please raise an issue to this repository, I will try to help as much as I can.

## PWM Fan Control
The PWM fan control service is responsible for controlling the fan speed depending on the CPU temperature. It reads the CPU temperature from the `/sys/class/thermal/thermal_zone0/temp` file once per second and saves it in a circular buffer, which contains last 80 temperature reads. To determine the fan speed, it reads the maximum value in the circular buffer and - depending on the range which the temperature falls into - sets the fan speed. Depending on your climate and isolation, adjustments to the temperature ranges may be required. The ones currently set in this repository are what I found the most optimal for my case.

## Power Control
The power control service is responsible for performing reboots or soft shutdowns of the server, which occur as a result of pressing the power button. The table below describes the behavior of the server:

| Button press time (s) | Server behavior                      |
| --------------------- | ------------------------------------ |
| 0.0 - 0.2             | Nothing                              |
| 0.2 - 0.6             | Reboot                               |
| 0.6 - 8.0             | Soft shutdown                        |
| 8+                    | Hard shutdown (handled by the board) |

## Soft reboot and soft shutdown
The soft reboot and soft shutdown executables simulate the button press for duration required to reboot or shutdown the server. You can run it manually by running `./soft_reboot` or `./soft_shutdown` in the terminal or by adding the repository directory to the `PATH` variable and running `soft_reboot` and `soft_shutdown` command.