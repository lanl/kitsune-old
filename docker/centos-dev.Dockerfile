FROM centos:7

ARG MAKE_J=16


#  - enable and reinstall docs (man/info pages) for centos newbies like me
#  - install a basic development environment
RUN \
 sed -i 's/\(tsflags=nodocs\)/# \1/' /etc/yum.conf \
 && yum reinstall -y '*' \
 && yum install -y man man-pages \
 && yum group install -y "Development Tools"

# Get a recent version of CMake  
RUN \
 git clone https://github.com/Kitware/CMake \
 && cd CMake \
 && ./bootstrap \
 && make -j ${MAKE_J}\
 && make install

# IMPORTANT: the context for `docker build` should be the kitsune project root
# COPY . .

# RUN                  \
#      mkdir -p build  \
#   && cd build        \
#   && cmake ..        \
#   && cmake --build . \
