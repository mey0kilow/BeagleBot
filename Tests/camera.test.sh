#!/bin/sh

vlc -I dummy v4l2://$1 --video-filter scene --no-audio --scene-path ./ --scene-prefix cameratest --scene-format png vlc://quit --run-time=1
