FROM alpine:latest

RUN apk update
RUN apk add alpine-sdk linux-headers autoconf flex bison ncurses-dev readline-dev 

WORKDIR /code