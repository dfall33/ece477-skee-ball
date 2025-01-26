import os
import struct
from machine import Pin, I2S, reset

# I2Ssetup
# reset()
sck_pin = Pin(14)  # serial clock
ws_pin = Pin(13)   # word clock
sd_pin = Pin(12)   # serial data output
audio_output = I2S(0, sck=sck_pin, ws=ws_pin, sd=sd_pin, mode=I2S.TX, bits=16, format=I2S.STEREO, rate=44100, ibuf=20000)

# Global variables
pcm_data_generator = None  # Initialize globally to prevent NameError
# wav_file = "400hz_3sec.wav"
wav_file = "mixkit-unlock-game-notification-253.wav"
wav_audio_format = 0
wav_num_channels = 0
wav_sample_rate = 0
wav_byte_rate = 0
wav_block_align = 0
wav_bits_per_sample = 0
wav_subchunk2_id = 0
wav_subchunk2_size = 0

# Load the WAV file and extract its data information
def load_wav_file(filename):
    global wav_audio_format, wav_byte_rate, wav_block_align, wav_subchunk2_id, wav_sample_rate, wav_bits_per_sample, wav_num_channels, wav_subchunk2_size, pcm_data_generator
    try:
        with open(filename, 'rb') as wav:
            data = wav.read(44)  # WAV data is 44 bytes
            wav_audio_format, wav_num_channels, wav_sample_rate, wav_byte_rate, wav_block_align, wav_bits_per_sample, wav_subchunk2_id, wav_subchunk2_size = struct.unpack('<HHIIHH4sI', data[20:44])          
            print("Sample Rate:", wav_sample_rate)
            print("Bits per Sample:", wav_bits_per_sample)
            print("Channels:", wav_num_channels)
            print("Data Size:", wav_subchunk2_size)
            print("Byte Rate:", wav_byte_rate)
            
            wav.seek(44)  # Move to PCM data section
            pcm_data_generator = read_pcm_data(filename, 256)  # Initialize the generator here
    except Exception as e:
        print("Error loading WAV file:", e)

# Function to read chunks of PCM data from the file
def read_pcm_data(filename, chunk_size):
    with open(filename, 'rb') as f:
        print(f)
        f.seek(44)  # Skip the data
        while True:
            chunk = f.read(chunk_size * wav_num_channels * 2) 
            if not chunk:
                break
            yield chunk  # Directly yield the array

# Function to play the next chunk of PCM data
def play_wav_12s():
    global pcm_data_generator, audio_output
    try:
        for pcm_chunk in pcm_data_generator:
            audio_output.write(pcm_chunk)
    except Exception as e:
        print("Error during playback:", e)
    finally:
        audio_output.deinit()  # Stop playback when done

def main():
    try:
        load_wav_file(wav_file)
        print('loaded wav')
        play_wav_12s()
    except Exception as e:
        print("Error loading WAV:", e)

if __name__ == "__main__":
    main()
    
