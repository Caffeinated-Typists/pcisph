#!/bin/bash

# Build the Docker image
docker build -t pcisph .

# Run the container with X11 forwarding
docker run --rm \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    --device /dev/dri \
    --device /dev/input \
    pcisph