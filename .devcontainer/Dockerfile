FROM ubuntu@sha256:79efa276fdefa2ee3911db29b0608f8c0561c347ec3f4d4139980d43b168d991

# Update the package database and install required tools
RUN apt-get clean
RUN apt-get update
RUN apt-get --fix-broken install
RUN apt-get install -y python3 python3-pip pipx git less vim sudo 
RUN apt-get install -y make g++-15 clang-format clangd-19 gnuplot

RUN apt-get install -y cmake

# Update  gcc/g++ version to the latest ones
RUN ln -fs /usr/bin/g++-15 /usr/bin/g++
RUN ln -fs /usr/bin/gcc-15 /usr/bin/gcc



# Copy conan profile
COPY default /home/ubuntu/.conan2/profiles/default
RUN chown -R ubuntu:ubuntu /home/ubuntu/.conan2
RUN chmod 666 /home/ubuntu/.conan2/profiles/default

USER ubuntu

# Install conan through pipx
RUN pipx install conan
RUN pipx ensurepath

RUN git config --global core.editor code

# Set the default command
CMD ["/bin/bash"]
