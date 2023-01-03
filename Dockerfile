FROM ubuntu AS build-libs

# Install dependencies
RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y cmake
RUN apt-get install -y build-essential gcc make
RUN apt-get install -y libssl-dev
RUN apt-get install -y doxygen graphviz
RUN git clone https://github.com/eclipse/paho.mqtt.c.git
RUN git clone https://github.com/eclipse/paho.mqtt.cpp

# Build libs
RUN bash -xc " \
    pushd paho.mqtt.c; \
    git checkout v1.3.8; \
    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON; \
    cmake --build build/ --target install; \
    ldconfig; \
    popd; \
    " \

RUN bash -xc" \
    pushd paho.mqtt.cpp; \
    cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE; \
    cmake --build build/ --target install; \
    ldconfig; \
    popd; \
    "

FROM ubuntu AS build-src
RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y build-essential gcc make
RUN ls -R /usr/local
COPY --from=build-libs usr/local/lib/libpaho-mqtt3a.so /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3a.so.1 /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3a.so.1.3.8 /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3as.so /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3a.so.1 /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3a.so.1.3.8 /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3c.so /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3c.so.1 /shared_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3c.so.1.3.8 /shared_libs_linux
#COPY --from=build-libs usr/local/lib/libpaho-mqttpp3.so /shared_libs_linux
#COPY --from=build-libs usr/local/lib/libpaho-mqttpp3.so.1 /shared_libs_linux
#COPY --from=build-libs usr/local/lib/libpaho-mqttpp3.so.1.2.0 /shared_libs_linux

#COPY --from=build-libs usr/local/lib/libpaho-mqtt3a.a /static_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3as.a /static_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3c.a /static_libs_linux
COPY --from=build-libs usr/local/lib/libpaho-mqtt3cs.a /static_libs_linux
#COPY --from=build-libs usr/local/lib/libpaho-mqttpp3.a /static_libs_linux

COPY --from=build-libs usr/local/include/* /include_linux/

#taking sources
COPY . .

#building sources
RUN mkdir "build"
RUN bash -xc "\
    pushd build; \
    cmake ..; \
    cmake --build . --target controller ; \
    "

FROM ubuntu

WORKDIR /
COPY --from=build-src /libs_shared/ /
COPY --from=build-src /build/controller /

CMD ["./controller", "sensor", "oxygen"]