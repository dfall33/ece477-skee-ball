from machine import Pin, Timer, SPI
import time 

class SOC1602A:
    
    SCK_PIN_NUM = 5
    MOSI_PIN_NUM = 19
    MISO_PIN_NUM = 21
    SPI_PERIPHERAL_NUM = 1
    CS_PIN_NUM = 15
    
    CLEAR_DISPLAY = 0x01
    FUNCTION_SET = 0x38
    DISPLAY_OFF = 0x08
    ENTRY_MODE = 0x06
    FIRST_LINE = 0x02
    SECOND_LINE= 0xC0
    DISPLAY_ON = 0x0C
    
    
    BAUDRATE = int(1e6)
    FRAME_LEN = 10
    
    def __init__(self):
        
        self.sck = Pin(
            self.SCK_PIN_NUM,
            Pin.OUT
        )
        
        self.mosi = Pin(
            self.MOSI_PIN_NUM,
            Pin.OUT
        )
        
        self.miso = Pin(
            self.MISO_PIN_NUM,
            Pin.OUT
        )

        self.cs = Pin(
            self.CS_PIN_NUM,
            Pin.OUT
        )
        
        self.spi = SPI(self.SPI_PERIPHERAL_NUM)
        self.spi.init(
            baudrate=self.BAUDRATE,
            bits=self.FRAME_LEN,
            sck=self.sck,
            mosi=self.mosi,
            miso=self.miso,
            firstbit=SPI.MSB
        )
        
        time.sleep(1)
        self.spi_cmd(self.FUNCTION_SET)
        self.spi_cmd(self.DISPLAY_OFF)
        self.spi_cmd(self.CLEAR_DISPLAY)
        time.sleep_ms(2)
        self.spi_cmd(self.ENTRY_MODE)
        self.spi_cmd(self.FIRST_LINE)
        self.spi_cmd(self.DISPLAY_ON)
    
    
    def prepare_frame(self, val: int) -> bytearray:
        val = (val & 0b1111111111) << 6
        return bytearray([val >> 8, val])
    
    
    def chip_select(self) -> None:
        self.cs.value(0)
    
    def chip_deselect(self) -> None:
        self.cs.value(1)
    
    
    def spi_cmd(self, cmd: int) -> None:
        self.chip_select()
        self.spi.write(
            self.prepare_frame(cmd)
        )
        self.chip_deselect()
            
    
    def spi_data(self, data: int) -> None:
        self.chip_select()
        data |= 0x200
        self.spi.write(
            self.prepare_frame(data)
        )
        self.chip_deselect()    
    
    
    def clear(self) -> None:
        self.cmd(
            SELF.CLEAR_DISPLAY
        )
        
        
    def write_string(self, string: str, line: int=0) -> None:
        if line == 0:
            self.spi_cmd(self.FIRST_LINE)
        elif line == 1:
            self.spi_cmd(self.SECOND_LINE)
        else:
            return
        
        for char in string:
            self.spi_data(ord(char))
        
