FROM debian:trixie
RUN apt-get update && apt-get install -y build-essential gdb valgrind \
    inetutils-ping tcpdump && rm -rf /var/lib/apt/lists/*
WORKDIR /app


# docker build -t ft_ping_dev .
# docker run -it --rm -v "$PWD":/app ft_ping_dev
