#!/usr/bin/env bash

cp oopetris.service /etc/systemd/system/

sudo systemctl daemon-reload
sudo systemctl enable --now oopetris


sudo systemctl status oopetris
