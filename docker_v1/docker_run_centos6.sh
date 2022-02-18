docker pull centos:centos6.10
docker run -it --rm centos:centos6.10 /bin/bash

# add repos  - https://www.getpagespeed.com/server-setup/how-to-fix-yum-after-centos-6-went-eol
curl https://www.getpagespeed.com/files/centos6-eol.repo --output /etc/yum.repos.d/CentOS-Base.repo
curl https://www.getpagespeed.com/files/centos6-epel-eol.repo --output /etc/yum.repos.d/epel.repo

# additional repos 
curl https://www.getpagespeed.com/files/centos6-scl-eol.repo --output /etc/yum.repos.d/CentOS-SCLo-scl.repo
curl https://www.getpagespeed.com/files/centos6-scl-rh-eol.repo --output /etc/yum.repos.d/CentOS-SCLo-scl-rh.repo


# replace some characters 
sed -i 's%^mirrorlist%#mirrorlist%g' /etc/yum.repos.d/* 
sed -i 's%#baseurl=http://mirror.centos.org%baseurl=https://vault.centos.org%g' /etc/yum.repos.d/*  # replace all mirrors with vault
sed -i 's,http://vault.centos.org,https://vault.centos.org,g' /etc/yum.repos.d/* # replace http with https for vault 

# add proxy 
echo "proxy=http://proxy.tch.harvard.edu:3128" >> /etc/yum.conf 

# install epel repository 
yum -y install epel-release