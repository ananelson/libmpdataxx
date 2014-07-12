README for dexy/docker/reproducibility

The run-docker.sh script creates a reproducible Docker container which has been configured by the Dockerfile.

Within this container, the run-dexy.sh script compiles the simulation code and copies files into locations where they can be more easily used by dexy.

Dexy runs the simualation and subsequent python scripts via the run-simulation.sh script.
