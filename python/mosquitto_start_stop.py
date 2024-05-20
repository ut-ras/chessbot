import subprocess
import atexit
import platform

def start_mosquitto():
    try:
        if platform.system() == "Darwin":  # macOS
            subprocess.run(["brew", "services", "start", "mosquitto"], check=True)
            print("Mosquitto service started on macOS.")
        elif platform.system() == "Linux":  # Linux
            subprocess.run(["sudo", "systemctl", "start", "mosquitto"], check=True)
            print("Mosquitto service started on Linux.")
        else:
            raise OSError("Unsupported operating system.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to start Mosquitto service: {e}")
        raise

def stop_mosquitto():
    try:
        if platform.system() == "Darwin":  # macOS
            subprocess.run(["brew", "services", "stop", "mosquitto"], check=True)
            print("Mosquitto service stopped on macOS.")
        elif platform.system() == "Linux":  # Linux
            subprocess.run(["sudo", "systemctl", "stop", "mosquitto"], check=True)
            print("Mosquitto service stopped on Linux.")
        else:
            raise OSError("Unsupported operating system.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to stop Mosquitto service: {e}")

# Register the stop_mosquitto function to be called on script exit
atexit.register(stop_mosquitto)

if __name__ == "__main__":
    try:
        start_mosquitto()
        while True:
            pass  #put stuff here later
        

    except KeyboardInterrupt:
        print("Script interrupted by user.")

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        stop_mosquitto()
