
# get messages per second on a topic
mosquitto_sub -t /led -v -F "" | pv --line-mode --rate -b --average-rate --timer > /dev/null

# scan all i2c devices on bus 0 (or 1)
sudo watch -n.1 i2cdetect -y 0