print("ultrasonic")
from machine import Pin, time_pulse_us
import machine
import time
ECHO_PIN_NUM = 33
TRIGGER_PIN_NUM = 4
echo = Pin(ECHO_PIN_NUM, Pin.IN)
trig = Pin(TRIGGER_PIN_NUM, Pin.OUT)

sound_velocity = 343 # m/s
timeout_distance = 1 # m
echo_timeout = timeout_distance / sound_velocity # time in s
echo_timeout *= 1e6    # time in us
echo_timeout = int(echo_timeout)
print(f"timeout: {echo_timeout}")
print("ultrasonic")

def duration_to_cm(duration):
    
    # duration in us
    # duration *= 1e-6 (seconds)
    # dist *= speed of sound (meters)
    # dist *= 10 (centimeters)
    # so dist = 1e-6 * speed of sound * 10 = 1e-5 * speed of sound
    return 1e-5 * sound_velocity * duration

def trigger():
    
    global trig
    trig.value(0)
    time.sleep_us(10)
    trig.value(1)
    time.sleep_us(10)
    trig.value(0)

def main():
    global trig, echo
    while True:
        trigger()
        duration_us = time_pulse_us(echo, 1, 30000)
        
        if duration_us == -1 or duration_us == -2:
            print(duration_us)
            time.sleep(0.5)
            continue
        cm = duration_to_cm(duration_us)
        print(cm)
        time.sleep(0.5)
        
    
    pass


if __name__ == "__main__":
    main()
    print("ultrasonic proto")
    #machine.soft_reset()
    
