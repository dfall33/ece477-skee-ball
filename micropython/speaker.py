import os
import struct
from machine import Pin, I2S, reset, Timer


# I2Ssetup
class Speaker:
    def __init__(self, wav_input):
        # reset()
        self.sck_pin = Pin(14)  # serial clock
        self.ws_pin = Pin(13)  # word clock
        self.sd_pin = Pin(12)  # serial data output
        self.audio_output = I2S(
            0,
            sck=self.sck_pin,
            ws=self.ws_pin,
            sd=self.sd_pin,
            mode=I2S.TX,
            bits=16,
            format=I2S.STEREO,
            rate=44100,
            ibuf=20000,
        )

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

    # Load the WAV file and extract its data information
    def load_wav_file(self):
        filename = self.wav_file
        # global self.wav_audio_format, self.wav_byte_rate, self.wav_block_align, self.wav_subchunk2_id, self.wav_sample_rate, self.wav_bits_per_sample, self.wav_num_channels, self.wav_subchunk2_size, self.pcm_data_generator
        try:
            with open(filename, "rb") as wav:
                data = wav.read(44)  # WAV data is 44 bytes
                (
                    self.wav_audio_format,
                    self.wav_num_channels,
                    self.wav_sample_rate,
                    self.wav_byte_rate,
                    self.wav_block_align,
                    self.wav_bits_per_sample,
                    self.wav_subchunk2_id,
                    self.wav_subchunk2_size,
                ) = struct.unpack("<HHIIHH4sI", data[20:44])
                print("Sample Rate:", self.wav_sample_rate)
                print("Bits per Sample:", self.wav_bits_per_sample)
                print("Channels:", self.wav_num_channels)
                print("Data Size:", self.wav_subchunk2_size)
                print("Byte Rate:", self.wav_byte_rate)

                wav.seek(44)  # Move to PCM data section
                self.pcm_data_generator = self.read_pcm_data(
                    filename, 256
                )  # Initialize the generator here
        except Exception as e:
            print("Error loading WAV file:", e)

    # Function to read chunks of PCM data from the file
    def read_pcm_data(self, filename, chunk_size):
        with open(filename, "rb") as f:
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
            self.audio_output = I2S(
                0,
                sck=self.sck_pin,
                ws=self.ws_pin,
                sd=self.sd_pin,
                mode=I2S.TX,
                bits=16,
                format=I2S.STEREO,
                rate=44100,
                ibuf=20000,
            )
            self.pcm_data_generator = self.read_pcm_data(self.wav_file, 256)
            for pcm_chunk in self.pcm_data_generator:
                if self.playing == False:
                    break
                self.audio_output.write(pcm_chunk)
        except Exception as e:
            print("Error during playback:", e)
        finally:
            self.audio_output.deinit()  # Stop playback when done


def main():
    global background_music, success_music

    # Initialize background and success sounds
    success_music = Speaker("success.wav")
    success_music.load_wav_file()
    success_music.play_wav()


if __name__ == "__main__":
    main()
