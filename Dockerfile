FROM gcc:latest

RUN apt update && \
    apt install -y libboost-test-dev

WORKDIR /mini_dev

ADD . /mini_dev

