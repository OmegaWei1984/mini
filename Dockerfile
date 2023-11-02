FROM alpine:latest

# RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories

RUN apk add build-base && \
    apk add boost-dev

WORKDIR /mini_dev

VOLUME /mini_dev
