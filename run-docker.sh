set -e
docker build -t dexy-experiments/cmake .
docker run -t -i \
    -v `pwd`:/home/repro/example \
    dexy-experiments/cmake /bin/bash
