set -e
docker build -t dexy-experiments/cmake .
docker run -t -i \
    -v `pwd`:/home/repro/example \
    -v /home/ana/dev/dexy:/home/repro/live-dexy \
    dexy-experiments/cmake /bin/bash
