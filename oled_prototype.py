from machine import Pin, Timer, SPI
import machine
import time

# Pin Definitions
SCK = 5
MOSI = 19
MISO = 21
SPI_PERIPHERAL_NUM = 0
SPI_CS = 15
DC_PIN = 32
RST_PIN = 4

# Pin Initializations
sck = Pin(SCK, Pin.OUT)
mosi = Pin(MOSI, Pin.OUT)
miso = Pin(MISO, Pin.OUT)

BAUDRATE = int(1e6)

def int_to_binary_bytes(integer):
    # Ensure input is within 10-bit range
    integer &= 0b1111111111  # Truncate to 10 bits
    
    # Convert integer to 10-bit binary representation
    binary = f"{integer:010b}"
    
    # Convert binary string to byte string with 00/01 representation
    byte_string = b"".join(b"\x00" if bit == '0' else b"\x01" for bit in binary)
    
    return byte_string

def truncate_to_10_bits(value):
    """
    Truncate a value to exactly 10 bits (0-1023 range)
    
    Args:
        value (int): The input value to truncate
    
    Returns:
        int: The value truncated to 10 bits
    """
    return value & 0b1111111111  # Bitwise AND with 10-bit mask (1023 in decimal)

def prepare_10bit_value(value):
    # Ensure value is within 10-bit range
    value &= 0b1111111111
    
    # Shift left by 6 bits to place 10-bit value in most significant bits
    value = value << 6
    return bytearray([value >> 8, value])

def view_bits(bytearr):
    """
    Convert a bytearray to a binary string representation
    
    Args:
        bytearr (bytearray): Input bytearray to convert
    
    Returns:
        str: Binary string representation
    """
    binary_str = ""
    for byte in bytearr:
        binary_str += f"{byte:08b}"
    
    return binary_str

# SPI Initialization
spi = SPI(1)
spi.init(
    baudrate=BAUDRATE,
    bits=1,
    sck=sck,
    mosi=mosi,
    miso=miso,
    firstbit=SPI.MSB
)
print(f"SPI: {spi}")

# Pin Initializations
cs = Pin(SPI_CS, Pin.OUT)
dc = Pin(DC_PIN, Pin.OUT)
rst = Pin(RST_PIN, Pin.OUT)

# Reset Sequence
rst.value(0)
time.sleep_ms(50)
rst.value(1)

def send_command(cmd):
    """
    Send a command over SPI with 10-bit truncation
    
    Args:
        cmd (int): Command to send
    """
    cs.value(0)
    
    # Truncate command to 10 bits
    cmd = prepare_10bit_value(cmd)
    print(f"cmd: {cmd}")
    
    spi.write(cmd)
    
    cs.value(1)

def send_data(data):
    """
    Send data over SPI with 10-bit truncation
    
    Args:
        data (int): Data to send
    """
    dc.value(1)
    cs.value(0)
    
    data |= 0x200
    data = prepare_10bit_value(data)
    print(f"data: {data}")
    
    spi.write(data)
    
    cs.value(1)

def init_display():
    """
    Initialize the display with standard commands
    """
    time.sleep_ms(1)
    send_command(0x38)
    
    time.sleep_ms(2)
    send_command(0x08)
    
    time.sleep_ms(2)
    send_command(0x01)
    

    time.sleep_ms(2)
    send_command(0x06)
    
    time.sleep_ms(2)
    send_command(0x02)
    
    time.sleep_ms(2)

    send_command(0x0C)

def write_text(text, line=0):
    """
    Write text to a specific line on the display
    
    Args:
        text (str): Text to write
        line (int, optional): Line number to write to. Defaults to 0.
    """
    if line == 0:
        send_command(0x02)
    elif line == 1:
        send_command(0xC0)
    
    for char in text:
        char = ord(char)
        print(f"char: {char}")
        send_data(char)

def main():
    """
    Main function to demonstrate display initialization and text writing
    """
    init_display()
    print("initialized")
    write_text("Firstline!", line=0)
    write_text("Secondline :)", line=1)
    print("done")

if __name__ == "__main__":
    main()
