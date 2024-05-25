#!/bin/sh
# dtparam=i2c_arm=on
# dtparam=i2s=on
# dtparam=spi=on
#
# # Enable audio (loads snd_bcm2835)
# #dtparam=audio=on

sudo systemctl enable --now ssh
sudo apt update
sudo apt install git vim mosquitto mosquitto-clients libi2c-dev python3-dev tmux pv
sudo systemctl enable --now mosquitto
git clone git@github.com:ut-ras/chessbot.git

cd chessbot

python -m venv venv
venv/bin/pip install -r requirements.txt
