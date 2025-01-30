import os
import struct
from machine import Pin, I2S, reset, Timer

# I2Ssetup
class I2S_setup:
    def __init__(self, wav_input):
        #reset()
        self.sck_pin = Pin(14)  # serial clock
        self.ws_pin = Pin(13)   # word clock
        self.sd_pin = Pin(12)   # serial data output
        self.audio_output = I2S(0, sck=self.sck_pin, ws=self.ws_pin, sd=self.sd_pin, mode=I2S.TX, bits=16, format=I2S.STEREO, rate=44100, ibuf=20000)

        # Global variables
        self.pcm_data_generator = None  # Initialize globally to prevent NameError
        # wav_file = "400hz_3sec.wav"
        self.wav_file = wav_input
        self.wav_audio_format = 0
        self.wav_num_channels = 0
        self.wav_sample_rate = 0
        self.wav_byte_rate = 0
        self.wav_block_align = 0
        self.wav_bits_per_sample = 0
        self.wav_subchunk2_id = 0
        self.wav_subchunk2_size = 0
        self.playing = False
        self.background_music = "8-bit-loop-music-290770.wav"
        self.score_success = "mixkit-unlock-game-notification-253.wav"

    # Load the WAV file and extract its data information
    def load_wav_file(self, filename):
        #global self.wav_audio_format, self.wav_byte_rate, self.wav_block_align, self.wav_subchunk2_id, self.wav_sample_rate, self.wav_bits_per_sample, self.wav_num_channels, self.wav_subchunk2_size, self.pcm_data_generator
        try:
            with open(filename, 'rb') as wav:
                data = wav.read(44)  # WAV data is 44 bytes
                self.wav_audio_format, self.wav_num_channels, self.wav_sample_rate, self.wav_byte_rate, self.wav_block_align, self.wav_bits_per_sample, self.wav_subchunk2_id, self.wav_subchunk2_size = struct.unpack('<HHIIHH4sI', data[20:44])          
                print("Sample Rate:", self.wav_sample_rate)
                print("Bits per Sample:", self.wav_bits_per_sample)
                print("Channels:", self.wav_num_channels)
                print("Data Size:", self.wav_subchunk2_size)
                print("Byte Rate:", self.wav_byte_rate)
                
                wav.seek(44)  # Move to PCM data section
                self.pcm_data_generator = self.read_pcm_data(filename, 256)  # Initialize the generator here
        except Exception as e:
            print("Error loading WAV file:", e)

    # Function to read chunks of PCM data from the file
    def read_pcm_data(self, filename, chunk_size):
        with open(filename, 'rb') as f:
            f.seek(44)  # Skip the data
            while True:
                chunk = f.read(chunk_size * self.wav_num_channels * 2) 
                if not chunk:
                    break
                yield chunk  # Directly yield the array

    # Function to play the next chunk of PCM data
    def play_wav(self):
        self.playing = True
        try:
            self.audio_output.deinit()
            self.audio_output = I2S(0, sck=self.sck_pin, ws=self.ws_pin, sd=self.sd_pin, mode=I2S.TX, bits=16, format=I2S.STEREO, rate=44100, ibuf=20000)
            self.pcm_data_generator = self.read_pcm_data(self.wav_file, 256)
            for pcm_chunk in self.pcm_data_generator:
                if self.playing == False:
                    break
                self.audio_output.write(pcm_chunk)
        except Exception as e:
            print("Error during playback:", e)
        finally:
            self.audio_output.deinit()  # Stop playback when done
            
    def stop_playing(self):
        self.playing = False
        self.audio_output.deinit()
        
    def loop_background_music(self):
        while True:
            self.load_wav_file(self.background_music)  # Load the WAV file
            self.play_wav()  # Play the WAV file repeatedly
        
def button_handler(pin):
    global background_music, success_music
         # Stop the background music
    background_music.stop_playing()

        # Play the success sound
    success_music.load_wav_file(success_music.score_success)
    success_music.play_wav()
    background_music.loop_background_music()
        
def main():
    global background_music, success_music

    # Initialize background and success sounds
    background_music = I2S_setup("8-bit-loop-music-290770.wav")
    success_music = I2S_setup("mixkit-unlock-game-notification-253.wav")
    
    # Set up the button (using GPIO pin 33 for button input)
    button_pin = Pin(33, Pin.IN, Pin.PULL_UP)
    button_pin.irq(trigger=Pin.IRQ_FALLING, handler=button_handler)
    
    # Set up a timer to loop the background music
    #background_music.load_wav_file(background_music.background_music)
    background_music.loop_background_music()
    #music_timer = Timer(-1)
    #music_timer.init(period=8000, mode=Timer.PERIODIC, callback=loop_background_music)


if __name__ == "__main__":
    main()
    

