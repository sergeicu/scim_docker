FROM centos:centos6.10 AS build

LABEL maintainer="serge@crl.med.harvard.edu"
LABEL vendor="Computational Radiology Laboratory"

# A word about HTTP_PROXY
# If you are building a docker image on a network that requires access to a proxy to download packages, e.g. the hospital, 
# you should set http_proxy in the following way 
# 
# docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $YOURDOCKERIMAGENAME . 
#

ENV LANGUAGE=en_US.UTF-8 \
    LANG=en_US.UTF-8 \
    LC_ALL=en_US.UTF-8 \
    LC_CTYPE=en_US.UTF-8

# Replace regular CentOS 6 mirror list with vault.centos.org, as CentOS 6 is EOL
# and regular mirrors aren't hosted any more.
RUN sed -i 's%^mirrorlist%#mirrorlist%g' /etc/yum.repos.d/* && \
    sed -i 's%#baseurl=http://mirror.centos.org%baseurl=http://vault.centos.org%g' /etc/yum.repos.d/* && \
    yum makecache fast 
#    && \
#    yum -y install gcc pam-devel glibc-devel net-tools tree git zip && \
#    yum clean all    
    





