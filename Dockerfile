FROM alpine:3.6
RUN mkdir app
WORKDIR app
COPY / /app
RUN apk add --update g++ && \
    rm /var/cache/apk/*
RUN gcc -o sensorapp main.c
CMD ["./sensorapp 192.168.2.138 1880 10 simsensor0001"]
