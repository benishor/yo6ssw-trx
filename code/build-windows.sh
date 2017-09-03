#!/bin/bash

cd docker
docker build -t stardust/mingw .
cd ..
docker run --rm -it -v $(pwd):/stardust stardust/mingw sh -c "./cross-compile.sh"
