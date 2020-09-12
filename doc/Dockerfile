FROM httpd:latest

RUN apt update \
 && apt install -y \
    python3 \
    python3-pip \
 && rm -rf /var/lib/apt/lists/*
RUN pip3 install sphinx \
 && rm -rf ~/.cache/pip/*
