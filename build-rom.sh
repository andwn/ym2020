#!/bin/sh

IMAGE_TAG="andwn/ym2020:latest"
IMAGE_NAME="ym2020"
GIT_REVISION=$(git rev-parse HEAD)

rm -f ym2020-*.bin

docker build \
	--tag ${IMAGE_TAG} \
	.

docker run \
	--volume "${PWD}:/src" \
	--rm \
	--name ${IMAGE_NAME} \
	${IMAGE_TAG} \
	/bin/bash -c "make clean all"

mv out.bin "ym2020-${GIT_REVISION}.bin"
