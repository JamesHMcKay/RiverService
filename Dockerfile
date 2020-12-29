FROM ubuntu:20.04
RUN apt-get update -y && apt-get install -y g++ make wget libboost-all-dev git
RUN apt-get install cmake
RUN apt-get install -y libssl-dev
RUN apt-get install -y libcpprest-dev
RUN wget https://github.com/zeux/pugixml/releases/download/v1.9/pugixml-1.9.tar.gz && tar xvzf pugixml-1.9.tar.gz && cd pugixml-1.9 && mkdir build && cd build && cmake .. && make && cd ../..
RUN mkdir -p RiverService/libs
ADD . /RiverService
RUN cd /RiverService && ls -la && ls ../pugixml-1.9/build/ && cmake . && cmake --build .
EXPOSE 8080
CMD ["bash", "-c", "./RiverService/crest $PORT"]
