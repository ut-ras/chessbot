
import RPi.GPIO as GPIO

from time import sleep
from gpiozero import Servo, Device
from gpiozero.pins.pigpio import PiGPIOFactory

# Set up GPIO pin factory to use PiGPIO for hardware PWM
Device.pin_factory = PiGPIOFactory()

# Define pins and setup
x_step_pin = 17  
x_dir_pin = 27   
y_step_pin = 22  
y_dir_pin = 5    
x_limit_switch_pin = 6  
y_limit_switch_pin = 13  
servo_pin = 18  # For servo motor 
electromagnet_pin = 23  # For electromagnet 

# Setup GPIO mode
GPIO.setmode(GPIO.BCM)
GPIO.setup([x_step_pin, x_dir_pin, y_step_pin, y_dir_pin, electromagnet_pin], GPIO.OUT)
GPIO.setup([x_limit_switch_pin, y_limit_switch_pin], GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Setup servo motor
servo = Servo(servo_pin)

# Initialize electromagnet state
GPIO.output(electromagnet_pin, GPIO.LOW)

# Constants
steps_per_mm = 10  # Placeholder value
board_origin_offset_mm = 50  # Distance from home to A1 square corner, Placeholder value
square_size_mm = 50

def home_axis(axis):
    if axis == 'x':
        GPIO.output(x_dir_pin, GPIO.LOW)  # Assume LOW moves towards home
        while GPIO.input(x_limit_switch_pin) == GPIO.HIGH:
            GPIO.output(x_step_pin, GPIO.HIGH)
            sleep(0.001)  # Short pulse duration
            GPIO.output(x_step_pin, GPIO.LOW)
            sleep(0.001)  # Step interval
    elif axis == 'y':
        GPIO.output(y_dir_pin, GPIO.LOW)  # Assume LOW moves towards home
        while GPIO.input(y_limit_switch_pin) == GPIO.HIGH:
            GPIO.output(y_step_pin, GPIO.HIGH)
            sleep(0.001)
            GPIO.output(y_step_pin, GPIO.LOW)
            sleep(0.001)

def home_axes(): # Call this function to home both axes
    home_axis('x')
    home_axis('y')


def square_to_coordinates(square): # Converts chess square to coordinates (e.g., "a1" -> (0, 0))
    # Converts chess square to coordinates (e.g., "a1" -> (0, 0))
    column = ord(square[0]) - ord('a')
    row = int(square[1]) - 1
    return (column * square_size_mm, row * square_size_mm)

def move_to_square(square):  # Move to a square on the board
    target_x, target_y = square_to_coordinates(square)
    current_x, current_y = 0, 0  # to track the current position
    
    steps_to_move_x = (target_x - current_x) * steps_per_mm
    steps_to_move_y = (target_y - current_y) * steps_per_mm
    
    # Move X Axis
    GPIO.output(x_dir_pin, GPIO.HIGH if steps_to_move_x > 0 else GPIO.LOW)
    for _ in range(abs(steps_to_move_x)):
        GPIO.output(x_step_pin, GPIO.HIGH)
        sleep(0.001)
        GPIO.output(x_step_pin, GPIO.LOW)
        sleep(0.001)
    
    # Move Y Axis
    GPIO.output(y_dir_pin, GPIO.HIGH if steps_to_move_y > 0 else GPIO.LOW)
    for _ in range(abs(steps_to_move_y)):
        GPIO.output(y_step_pin, GPIO.HIGH)
        sleep(0.001)
        GPIO.output(y_step_pin, GPIO.LOW)
        sleep(0.001)

    # Update current position
    current_x = target_x
    current_y = target_y


# Function to pick up or release a piece
def manipulate_piece_EM(engage):
    if engage:
        GPIO.output(electromagnet_pin, GPIO.HIGH)
    else:
        GPIO.output(electromagnet_pin, GPIO.LOW)

def manipulate_piece_servo(engage):
    if engage:
        servo.min()
    else:
        servo.max()

# Function to perform a move in UCI format
def perform_uci_move(uci_move):
    from_square, to_square = uci_move[:2], uci_move[2:]
    move_to_square(from_square)  # Move above the piece to pick up
    manipulate_piece_servo(True)  # Pick up the piece
    move_to_square(to_square)  # Move to the destination square
    manipulate_piece_servo(False)  # Release the piece

def perform_castling(uci_castling):
    # Move the king, then the rook
    if uci_castling == "e1g1":
        move_to_square("e1")
        manipulate_piece_servo(True)
        move_to_square("g1")
        manipulate_piece_servo(False)
        move_to_square("h1")
        manipulate_piece_servo(True)
        move_to_square("f1")
        manipulate_piece_servo(False)
    elif uci_castling == "e1c1":
        move_to_square("e1")
        manipulate_piece_servo(True)
        move_to_square("c1")
        manipulate_piece_servo(False)
        move_to_square("a1")
        manipulate_piece_servo(True)
        move_to_square("d1")
        manipulate_piece_servo(False)
    elif uci_castling == "e8g8":
        move_to_square("e8")
        manipulate_piece_servo(True)
        move_to_square("g8")
        manipulate_piece_servo(False)
        move_to_square("h8")
        manipulate_piece_servo(True)
        move_to_square("f8")
        manipulate_piece_servo(False)
    elif uci_castling == "e8c8":
        move_to_square("e8")
        manipulate_piece_servo(True)
        move_to_square("c8")
        manipulate_piece_servo(False)
        move_to_square("a8")
        manipulate_piece_servo(True)
        move_to_square("d8")
        manipulate_piece_servo(False)



if __name__ == "__main__":
    try:
        home_axes()
        perform_uci_move("e2e4")
    except KeyboardInterrupt:
        pass
    finally:
        GPIO.cleanup()
