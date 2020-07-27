ARG image_tag
FROM gcc:${image_tag}

ENV C_INCLUDE_PATH=/usr/include/postgresql:$C_INCLUDE_PATH
ENV CPLUS_INCLUDE_PATH=/usr/include/postgresql:$CPLUS_INCLUDE_PATH
ENV LIBRARY_PATH=/usr/local/lib64:/usr/local/lib:$LIBRARY_PATH
ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib:$LD_LIBRARY_PATH
ENV MSGPACK_VERSION 0.5.9

RUN apt update \
 && apt install -y \
    python3 \
    postgresql-server-dev-all \
 && rm -rf /var/lib/apt/lists/*
# Install msgpack
RUN wget https://github.com/msgpack/msgpack-c/releases/download/cpp-${MSGPACK_VERSION}/msgpack-${MSGPACK_VERSION}.tar.gz \
 && tar zxvf msgpack-${MSGPACK_VERSION}.tar.gz \
 && cd msgpack-${MSGPACK_VERSION} \
 && ./configure \
 && make \
 && make install \
 && cd .. \
 && rm -rf msgpack-${MSGPACK_VERSION} \
 && rm -f msgpack-${MSGPACK_VERSION}.tar.gz
# Install fcgi
ADD ./patches /patches
RUN wget https://github.com/FastCGI-Archives/FastCGI.com/raw/master/original_snapshot/fcgi-2.4.1-SNAP-0910052249.tar.gz \
 && tar zxvf fcgi-2.4.1-SNAP-0910052249.tar.gz \
 && cd fcgi-2.4.1-SNAP-0910052249 \
 && patch -u libfcgi/fcgio.cpp < /patches/fcgi.patch \
 && ./configure \
 && make \
 && make install \
 && cd .. \
 && rm -rf fcgi-2.4.1-SNAP-0910052249 \
 && rm -f fcgi-2.4.1-SNAP-0910052249.tar.gz
