FROM alpine:3.6
RUN mkdir app
WORKDIR app
COPY / /app
