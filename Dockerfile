FROM ubuntu:16.04
RUN apt-get update -y && apt-get install -y g++ make wget libboost-all-dev git
RUN wget https://cmake.org/files/v3.9/cmake-3.9.4.tar.gz && tar xvzf cmake-3.9.4.tar.gz && cd cmake-3.9.4 && ./bootstrap && make && make install && cd .. && rm -rf cmake-3.9.4
RUN apt-get install -y libssl-dev
RUN wget https://github.com/zeux/pugixml/releases/download/v1.9/pugixml-1.9.tar.gz && tar xvzf pugixml-1.9.tar.gz && cd pugixml-1.9 && mkdir build && cd build && cmake .. && make && cd ../..
RUN mkdir -p app/libs
ADD libs/build_dependencies.sh app/libs
RUN cd app/libs && ./build_dependencies.sh
ADD . /app
RUN cd /app && ls -la && ls ../pugixml-1.9/build/ && cmake . && cmake --build .
EXPOSE 8080
CMD ["bash", "-c", "./app/crest $PORT"]