FROM centos:centos6.10 AS build

# Replace regular CentOS 6 mirror list with vault.centos.org, as CentOS 6 is EOL
# and regular mirrors aren't hosted any more.
RUN sed -i 's%^mirrorlist%#mirrorlist%g' /etc/yum.repos.d/* && \
    sed -i 's%#baseurl=http://mirror.centos.org%baseurl=https://vault.centos.org%g' /etc/yum.repos.d/* && \
    yum makecache fast && \
    yum install -y git wget 
    yum clean all    
    
    
#yum -y install gcc pam-devel glibc-devel net-tools tree git zip && \
    





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


yum makecache fast 

yum clean all     # final step 

yum install -y wget # test 
  
yum -y install centos-release-scl # newer builds 



# LINE BY LINE INTERPRETATION OF UBUNTU AND CENTOS REQUIREMENTS 

yum groupinstall 'Development Tools' # build-essential 



build-essential # yum groupinstall 'Development Tools'  

        OR yum install gcc gcc-c++ make # https://unix.stackexchange.com/questions/16422/cant-install-build-essential-on-centos

apt-utils # yum update && yum install yum-utils
zlib1g-dev #  yum install zlib-devel # ? 
libncurses5-dev # yum install ncurses-devel
libgdbm-dev \
libnss3-dev \
libssl-dev \
libreadline-dev \
libffi-dev \
libxcb-xinerama0 #  dnf -y install libxcb # ? 
libxcb-xinerama0-dev \
wget # same 
git  # same 
perl # same 
python3 # yum install --nogpgcheck python34
python3-dev # could not find # yum search python3 | grep devel
python3-pip # could not find 
libproj-dev #  yum install libprojectM-devel
gperf bison flex \
unzip # same? 
cmake # same? 
libgoogle-glog-dev libgflags-dev \
libatlas-base-dev \ # atlas-devel?
libeigen3-dev \ # eigen3-devel
libsuitesparse-dev \ # suitesparse
qt5-default \ # qt5-...something
libtbb-dev \ # tbb-devel
libfreetype6-dev \ # not found 
libfontconfig-dev \ # fontconfig-devel
libdouble-conversion-dev \ # not found 
liblz4-dev liblzma-dev \ # lz4-devel       xz-lzma-compat
libnetcdf-dev libnetcdf-cxx-legacy-dev \
libogg-dev \ # 
libtheora-dev # libtheora-devel
libpng-dev \ libpng-devel
libjpeg-dev \openjpeg-devel ? 
libtiff-dev \ libtiff-devel
libjsoncpp-dev \ jsoncpp-devel
libexpat1-dev \ compat-expat1 
libglew-dev \ glew-devel
libhdf5-dev \ hdf5-devel
libqt5x11extras5-dev \ not found 
libqt5opengl5-dev \ not found 
qtbase5-dev \ qt5-qtbase
qttools5-dev \ qt5-qttools
libxt-dev libxml2-dev \ libXt-devel   libxml2-devel
libsqlite3-dev \ sqlite2 !! slite-devel 
libswscale-dev \ not found 
&& rm -rf /var/lib/apt/lists/*

