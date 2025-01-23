from machine import Pin, Timer, ADC

angle = 0

x_dir = ADC(Pin(34))
x_dir.atten(ADC.ATTN_11DB)  # Set attenuation to handle a 3.3V range
x_dir.width(ADC.WIDTH_12BIT) 

def read_joystick(t):
    adc_val = x_dir.read()
    degrees = map_adc_to_degrees(adc_val)
    print("XDIR: {}		DEGREES: {}".format(adc_val, degrees))

def map_adc_to_degrees(adc_val, adc_min=0, adc_max=4095, deg_min=-45, deg_max=45):
   degrees = (adc_val - adc_min) * (deg_max - deg_min) / (adc_max - adc_min) + deg_min
   return degrees


timer = Timer(0)
timer.init(period=1000, mode=Timer.PERIODIC, callback=read_joystick) 
