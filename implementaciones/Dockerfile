FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    openmpi-bin \
    openmpi-common \
    libopenmpi-dev \
    openssh-server \
    openssh-client \
    && rm -rf /var/lib/apt/lists/*

# SSH config
RUN mkdir /var/run/sshd
RUN echo 'root:mpi' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

RUN ssh-keygen -A
RUN ssh-keygen -t rsa -N "" -f /root/.ssh/id_rsa
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
RUN chmod 600 /root/.ssh/authorized_keys

RUN echo "Host *\n\tStrictHostKeyChecking no\n\tUserKnownHostsFile /dev/null" >> /root/.ssh/config

WORKDIR /app

# Copiar todo (fuentes, headers e imágenes) al contenedor
COPY . /app

# ⚡ Compilar SOLO en la imagen (master)
RUN mpic++ -std=c++11 -Wall -Wextra -O2 -I. -o mpi_filterer \
    mpiFilterer.cpp image.cpp imagesPGM.cpp imagesPPM.cpp \
    filter.cpp blurFilter.cpp laplaceFilter.cpp sharpenFilter.cpp timer.cpp

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
