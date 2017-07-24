#to build on docker host first: docker build --no-cache=true -f Dockerfile https://github.com/brianr82/sensorsim.git -t brianr82/sensorsim:latest
#usage example: docker run -it -e PI_IP='10.12.7.45' -e PI_PORT='1880' -e NUM_MSG='100' -e SENSOR_ID='simsensor001' singlesensorsim 
FROM alpine:3.6
RUN mkdir app
WORKDIR app
COPY / /app
RUN apk add --update g++ && \
    rm /var/cache/apk/*
RUN gcc -o sensorapp main.c
CMD ./sensorapp $PI_IP $PI_PORT $NUM_MSG $SENSOR_ID $DELAY
