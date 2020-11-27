FROM centos:centos7

ENV POSTGRESQL_VERSION 10
ENV MSGPACK_VERSION 0.5.9

ENV POSTGRESQL_PATH=/usr/pgsql-${POSTGRESQL_VERSION}
ENV PATH=${POSTGRESQL_PATH}/bin:$PATH
ENV C_INCLUDE_PATH=${POSTGRESQL_PATH}/include
ENV CPLUS_INCLUDE_PATH=${POSTGRESQL_PATH}/include
ENV LIBRARY_PATH=${POSTGRESQL_PATH}/lib:/usr/local/lib64:/usr/local/lib
ENV LD_LIBRARY_PATH=${POSTGRESQL_PATH}/lib:/usr/local/lib64:/usr/local/lib

RUN yum update -y \
 && yum install -y \
    @base \
    @development \
    python36 \
 && yum clean all \
 && rm -rf /var/cache/yum/*
# Install mysql
RUN yum localinstall -y \
    http://dev.mysql.com/get/mysql57-community-release-el7-11.noarch.rpm \
 && yum install -y \
    mysql-devel \
 && yum clean all \
 && rm -rf /var/cache/yum/*
# Install postgresql
# https://www.postgresql.org/download/linux/redhat/
RUN yum localinstall -y \
    https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-x86_64/pgdg-redhat-repo-latest.noarch.rpm \
 && yum install -y \
    postgresql${POSTGRESQL_VERSION}-devel \
 && yum clean all \
 && rm -rf /var/cache/yum/*
# Install msgpack
RUN wget https://github.com/msgpack/msgpack-c/releases/download/cpp-${MSGPACK_VERSION}/msgpack-${MSGPACK_VERSION}.tar.gz \
 && tar zxvf msgpack-${MSGPACK_VERSION}.tar.gz \
 && pushd msgpack-${MSGPACK_VERSION} \
 && ./configure \
 && make \
 && make install \
 && popd \
 && rm -rf msgpack-${MSGPACK_VERSION} \
 && rm -f msgpack-${MSGPACK_VERSION}.tar.gz
# Install fcgi
ADD ./patches /patches
RUN wget https://github.com/FastCGI-Archives/FastCGI.com/raw/master/original_snapshot/fcgi-2.4.1-SNAP-0910052249.tar.gz \
 && tar zxvf fcgi-2.4.1-SNAP-0910052249.tar.gz \
 && pushd fcgi-2.4.1-SNAP-0910052249 \
 && patch -u libfcgi/fcgio.cpp < /patches/fcgi.patch \
 && ./configure \
 && make \
 && make install \
 && popd \
 && rm -rf fcgi-2.4.1-SNAP-0910052249 \
 && rm -f fcgi-2.4.1-SNAP-0910052249.tar.gz
