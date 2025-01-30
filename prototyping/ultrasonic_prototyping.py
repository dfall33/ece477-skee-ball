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

current_sensor_id = 0 # 0 through 5
num_sensors = 6 # 6 total sensors, labeled 0 through 5

def duration_to_cm(duration):
    
    # duration in us
    # duration *= 1e-6 (seconds)
    # dist *= speed of sound (meters)
    # dist *= 10 (centimeters)
    # so dist = 1e-6 * speed of sound * 10 = 1e-5 * speed of sound
    return 1e-5 * sound_velocity * duration

def trigger():
    
    global trig, current_sensor_id, num_sensors 
    trig.value(0)
    time.sleep_us(10)
    trig.value(1)
    time.sleep_us(10)
    trig.value(0)
    current_sensor_id = (current_sensor_id + 1) % num_sensors

def main():
    global trig, echo, current_sensor_id
    num_cycles = 20
    count = 0
    while count < num_cycles * num_sensors:
        trigger()
        duration_us = time_pulse_us(echo, 1, 30000)
        pause = 1000
        if duration_us == -1 or duration_us == -2:
            print(f"Sensor={current_sensor_id}, error={duration_us}")
        else: 
            cm = duration_to_cm(duration_us)
            print(f"Sensor={current_sensor_id}, distance={cm}cm")
        time.sleep_us(pause)
        count += 1
        
    
    pass


if __name__ == "__main__":
    main()
    
