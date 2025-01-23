import machine
from machine import Pin, Timer
import time

PB_GPIO_PIN = 26
MAX_BUTTON_PRESS_DURATION = 5000


button_pin = Pin(PB_GPIO_PIN, Pin.IN, Pin.PULL_DOWN)

prev_interrupt_time = time.ticks_ms()

is_button_pressed = False

button_press_timer = Timer(0)

BOUNCE_DURATION = 100  # ms

button_release_enabled = True


def drive_motor(duration_ms):
    print(f"Powering motor from button press with duration {duration_ms}")


def button_overflow_isr(timer):

    global button_release_enabled
    button_release_enabled = False
    drive_motor(duration_ms=MAX_BUTTON_PRESS_DURATION)


def button_press_isr(timer):

    global button_pin, prev_interrupt_time, is_button_pressed, button_press_timer, button_release_enabled
    current_interrupt_time = time.ticks_ms()

    if time.ticks_diff(current_interrupt_time, prev_interrupt_time) > BOUNCE_DURATION:

        if is_button_pressed and not button_release_enabled:
            prev_interrupt_time = current_interrupt_time
            is_button_pressed = False
            button_release_enabled = True
            return

        is_button_pressed = not is_button_pressed

        if is_button_pressed:
            prev_interrupt_time = current_interrupt_time

            button_press_timer.deinit()
            button_press_timer.init(
                period=MAX_BUTTON_PRESS_DURATION,
                mode=Timer.ONE_SHOT,
                callback=button_overflow_isr,
            )
        else:

            if button_release_enabled:
                button_press_timer.deinit()
                duration = time.ticks_diff(current_interrupt_time, prev_interrupt_time)
                prev_interrupt_time = current_interrupt_time

                drive_motor(duration)


def main():

    global button_pin
    irq_trigger = Pin.IRQ_RISING | Pin.IRQ_FALLING
    button_pin.irq(trigger=irq_trigger, handler=button_press_isr)


if __name__ == "__main__":
    main()
