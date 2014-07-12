FROM                    phusion/baseimage
MAINTAINER              Ana Nelson <ana@ananelson.com>

### "localedef"
RUN localedef -v -c -i en_US -f UTF-8 en_US.UTF-8 || :

### "squid-deb-proxy"
# Use squid deb proxy (if available on host OS) as per https://gist.github.com/dergachev/8441335
# Modified by @ananelson to detect squid on host OS and only enable itself if found.
ENV HOST_IP_FILE /tmp/host-ip.txt
RUN /sbin/ip route | awk '/default/ { print "http://"$3":8000" }' > $HOST_IP_FILE
RUN HOST_IP=`cat $HOST_IP_FILE` && curl -s $HOST_IP | grep squid && echo "found squid" && echo "Acquire::http::Proxy \"$HOST_IP\";" > /etc/apt/apt.conf.d/30proxy || echo "no squid"

### "apt-defaults"
RUN echo "APT::Get::Assume-Yes true;" >> /etc/apt/apt.conf.d/80custom
RUN echo "APT::Get::Quiet true;" >> /etc/apt/apt.conf.d/80custom

### "update"
RUN apt-get update

### "utils"
RUN apt-get install build-essential
RUN apt-get install adduser sudo
RUN apt-get install curl

### "nice-things"
RUN apt-get install ack-grep strace vim git tree wget unzip rsync man-db

### "cmake"
RUN apt-get install cmake
RUN apt-get install clang

### "lib-deps"
RUN apt-get install libblitz0-dev
RUN apt-get install libboost-all-dev
RUN apt-get install libhdf5-dev
RUN apt-get install libgnuplot-iostream-dev

### "texlive"
RUN apt-get install --no-install-recommends texlive-latex-base
RUN apt-get install --no-install-recommends texlive-latex-extra

### "python"
RUN apt-get install python-dev
RUN apt-get install python-pip

### "dexy"
RUN pip install dexy

### "scipy"
RUN apt-get install python-scipy
RUN apt-get install python-matplotlib

### "python-libs"
RUN pip install h5py

### "xvfb"
RUN apt-get install xvfb

### "create-user"
RUN useradd -m repro
RUN echo "repro:foobarbaz" | chpasswd
RUN adduser repro sudo

### "activate-user"
ENV HOME /home/repro
USER repro
WORKDIR /home/repro
