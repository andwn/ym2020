FROM debian:bullseye-slim@sha256:5cf1d98cd0805951484f33b34c1ab25aac7007bb41c8b9901d97e4be3cf3ab04

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

RUN apt-get update && \
    apt-get install \
        --assume-yes \
        --no-install-recommends \
        --quiet=2 \
	ca-certificates=20210119 \
        curl=7.74.0-1.3+deb11u3 \
        make=4.3-4.1 \
        openjdk-11-jre=11.0.16+8-1~deb11u1 \
        xz-utils=5.2.5-2.1~deb11u1 && \
    rm -rf /var/lib/apt/lists/*

RUN curl --silent \
        --location \
        --remote-name https://github.com/andwn/marsdev/releases/download/2022.10/mars-2022.10-linux-x64.tar.xz && \
    echo 'cabe798e396b19a5f3ba46a4b3c75e3fe2176936f8ddc4f9e0a4d3d3de2f5b4b  mars-2022.10-linux-x64.tar.xz' | sha256sum --check && \
    tar --extract \
	--file mars-2022.10-linux-x64.tar.xz \
	--directory "${HOME}"

WORKDIR /src
