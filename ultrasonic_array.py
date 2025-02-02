from machine import Pin, time_pulse_us, Timer
import machine
import time


class UltrasonicArray:

    def __init__(
        self,
        echo_pin_number,
        trigger_pin_number,
        timeout_timer_number,
        num_sensors=6,
        sound_speed=343,
        threshold_cm=20,
        timeout_seconds=5,
        gap_time_us=1000,
        debug=False,
    ):

        self._NUM_SENSORS = num_sensors
        self._SOUND_SPEED = sound_speed
        self._GAP_TIME_US = gap_time_us
        self._TIMEOUT_MS = int(1000 * timeout_seconds)
        self._TIMEOUT_TIMER_NUMBER = timeout_timer_number
        self._DURATION_THRESHOLD_US = (
            threshold_cm * 1e4 / self._SOUND_SPEED
        )  # cm /= 100 = meters, meters / speed of sound = seconds, seconds * 1e6 = us, so us = 1e6/100/speed = 1e4 /speed
        self._PULSE_TIMEOUT = int(2 * self._DURATION_THRESHOLD_US)
        self._NOT_FOUND = -1
        self._DEBUG = debug

        self.echo_pin_number = echo_pin_number
        self.trigger_pin_number = trigger_pin_number

        self.echo = Pin(echo_pin_number, Pin.IN)
        self.trigger = Pin(trigger_pin_number, Pin.OUT)

        self._current_sensor_id = 0
        self.timed_out = False

        self.timeout_timer = Timer(self._TIMEOUT_TIMER_NUMBER)
        self.timeout_timer.init(
            mode=Timer.ONE_SHOT, period=self._TIMEOUT_MS, callback=self._timeout_isr
        )

    def _send_trigger(self):
        self.trigger.value(0)
        time.sleep_us(10)
        self.trigger.value(1)
        time.sleep_us(10)
        self.trigger.value(0)

    def _us_to_cm(self, dur):

        return 1e-4 * self._SOUND_SPEED * 0.5 * dur

    def _timeout_isr(self, timer):
        self.timed_out = True

    def _increment_id(self):
        self._current_sensor_id = (self._current_sensor_id + 1) % self._NUM_SENSORS

    def _reset_counter(self):
        num_pulses = (self._NUM_SENSORS - self._current_sensor_id) % self._NUM_SENSORS

        for i in range(num_pulses):
            self._send_trigger()

        self._current_sensor_id = 0

    def find_ball(self):

        while True:

            if self.timed_out:
                self.timeout_timer.deinit()
                self._reset_counter()
                self._current_sensor_id = 0
                return self._NOT_FOUND

            self._send_trigger()
            self._increment_id()
            duration_us = time_pulse_us(self.echo, 1, self._PULSE_TIMEOUT)

            if duration_us <= self._DURATION_THRESHOLD_US and duration_us > 0:
                cur = self._current_sensor_id
                self.timeout_timer.deinit()
                self._reset_counter()
                return cur

            time.sleep_us(self._GAP_TIME_US)
