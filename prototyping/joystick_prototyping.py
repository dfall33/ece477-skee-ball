from machine import Pin, Timer, ADC, PWM
import time, utime

X_DIR_PIN = 34
Y_DIR_PIN = 39
PWM_PIN = 25

x_dir = ADC(Pin(X_DIR_PIN))
x_dir.atten(ADC.ATTN_11DB)  # Set attenuation to handle a 3.3V range
x_dir.width(ADC.WIDTH_12BIT)

#from package DIYables_MicroPython_Servo
class Servo:
    def __init__(self, pin_number, min_pulse=500, max_pulse=2500, frequency=50):
        """ Initialize a servo object on a specified pin with customizable pulse widths and frequency. """
        self.servo_pin = PWM(Pin(pin_number))  # Set up PWM on the pin
        self.servo_pin.freq(frequency)         # Set frequency, default is 50Hz
        self.min_pulse = min_pulse
        self.max_pulse = max_pulse
        self.cycle = 1 / frequency * 1000000     # PWM cycle in us

    def move_to_angle(self, angle):
        """ Move the servo to a specific angle. """
        # Calculate the pulse width corresponding to the angle
        pulse_width = self.min_pulse + (self.max_pulse - self.min_pulse) * angle / 180
        
        # Set the PWM duty cycle
        self.servo_pin.duty_u16(int(pulse_width * 65536 / self.cycle))  # Convert to duty for Pico's 16-bit resolution
        print("DUTY {}".format(int(pulse_width * 65536 / self.cycle)))

    def deinit(self):
        """ Disable the PWM signal. """
        self.servo_pin.deinit()

motor = Servo(PWM_PIN)

def read_joystick(degrees):
    adc_val = x_dir.read()
    inc_degrees = map_adc_to_degrees(adc_val)
    if (degrees + inc_degrees) <=180 and (degrees +inc_degrees) >= 0:
        degrees += inc_degrees
    motor.move_to_angle(degrees)
    print("XDIR: {}		DEGREES: {}, {}".format(adc_val, degrees, inc_degrees))
    return degrees

def map_adc_to_degrees(adc_val, adc_min=0, adc_max=4095, deg_min=-4, deg_max=4):
   degrees = (adc_val - adc_min) * (deg_max - deg_min) / (adc_max - adc_min) + deg_min
   return degrees


#degrees = 45
def main():
    #global degree
    #timer = Timer(0)
    #timer.init(period=1000, mode=Timer.PERIODIC, callback=read_joystick)
    degrees = 45
    try:
        while(True):
            new_degrees = read_joystick(degrees)
            degrees = new_degrees
            time.sleep(.1)
    except KeyboardInterrupt:
        print("Exiting...")
        
if __name__ == '__main__':
    main()
