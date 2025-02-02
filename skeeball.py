from speaker import Speaker
from ultrasonic_array import UltrasonicArray 
from soc1602a import SOC1602A
from launch_button import LaunchButton
import time

class SkeeBall:
    
    READING_BUTTON_STATE = 0
    READING_JOYSTICK_STATE = 1 
    LAUNCHING_BALL_STATE = 2
    DETECTING_BALL_STATE = 3
    IDLE_STATE = 4
    ACTIVE_STATE = 5 
    
    MAX_ATTEMPTS = 3 

    
    def __init__(self):
        
        self.state = self.IDLE_STATE
        self.launch_power = 0
        
        self.remaining_attempts = self.MAX_ATTEMPTS
        
        self.high_score = "XX"
        
        self.sensors = UltrasonicArray(
            echo_pin_number=33,
            trigger_pin_number=4,
            timeout_timer_number = 0,
            threshold_cm=5,
            gap_time_us=100000
        )
        
        self.button = LaunchButton(
            pin_number=26,
            handle_release=self.handle_release, 
            handle_press=self.handle_press_down, 
            handle_poll=self.poll_button, 
            timeout_timer_number=2,
            poll_timer_number=1, 
        )
        
        self.speaker = Speaker(wav_input="success.wav")
        self.speaker.load_wav_file()
        self.screen = SOC1602A()
        
        self.current_score = 0 
        self.show_leaderboard()
        
        self._SCORE_TABLE = [23, 13, 64, 3, 48, 32]
        
        # do some setup, may have to add more later
        # ...
        
        # enable the button / launching the ball
        self.button.enable()
    
    def attempt_score(self):
        
        print('attempt_score() start')
        
        time.sleep(0.1)
        found_sensor = self.sensors.find_ball()
        
        if found_sensor != -1:
            print(f'found on {found_sensor}')
            self.speaker.play_wav()
            self.current_score += self._SCORE_TABLE[found_sensor]
            self._update_score()
        else:
            print("missed or not found")
            
        # attempt is over, so you can use the button again 
        self.button.enable()
        
        self.remaining_attempts -= 1 
        
        
    def handle_press_down(self):
        self.update_game_state(self.READING_BUTTON_STATE)
        self.display.clear()
        self.screen.write_progress(0.)
        
        
    def handle_release(self, power):
        print(f'handling relaase with power={power}')
        self.update_game_state(self.LAUNCHING_BALL_STATE)
        self.launch_power = power
        
        
    def update_game_state(self, state):
        
        print(f'setting state={state}')
        prev_state = self.state 
        self.state = state
        
        
        # when starting new turn, give max number of tries 
        if self.state != self.IDLE_STATE and prev_state == self.IDLE_STATE:
            self.remaining_attempts = self.MAX_ATTEMPTS
            self.current_score = 0
        
        # when there is no active turn, show the leaderboard/high score/whatever 
        if state == self.IDLE_STATE:
            self.show_leaderboard()
            
        # if currently in a turn, show the current score 
        elif state == self.ACTIVE_STATE:
            self._update_score()
            
        
    def launch_ball(self):
        
        # no more launching ball / using motor until this is done 
        self.button.disable()
        
        # get rid of the progress bar 
        self.screen.clear()
        
        # display the score again 
        self._update_score()
        
        # ----- LAUNCH BALL HERE ------
        print(f'launching ball with power={self.launch_power}')
        
    def poll_button(self, progress):
        
        # measure press duration at regular intervals and use the press progress for the screen progress bar 
        self.screen.write_progress(progress)
    
    def _update_score(self):
        self.screen.write_string(f"Score: {self.current_score}")
        self.screen.write_string(f"{self.remaining_attempts} tries left", line=1)
        
    def show_leaderboard(self):
        self.screen.write_string("Press to Play", line=0)
        self.screen.write_string(f"High Score: {self.high_score}", line=1)
        
    def has_remaining_attempts(self):
        return self.remaining_attempts != 0
    
    
    def show_final_score(self):
        self.screen.write_string(f'Final: {self.current_score}')
        time.sleep(5)
        
    def play(self):
        while True:
            
            if self.state == self.ACTIVE_STATE:
                if not self.has_remaining_attempts():
                    print('out of attempts')
                    self.show_final_score() # let them view their score for a bit before it goes away 
                    self.update_game_state(self.IDLE_STATE)
            
            if self.state == self.IDLE_STATE:
                continue
            
            elif self.state ==  self.LAUNCHING_BALL_STATE:
                self.launch_ball()
                self.update_game_state(self.DETECTING_BALL_STATE)
            
            elif self.state == self.DETECTING_BALL_STATE:
                self.attempt_score()
                self.update_game_state(self.ACTIVE_STATE)
                
            elif self.state == self.READING_BUTTON_STATE:
                continue
            
            
    
    
def main():
    game = SkeeBall()
    game.play()
    #game.attempt_score()
    
if __name__ == "__main__":
    main()
