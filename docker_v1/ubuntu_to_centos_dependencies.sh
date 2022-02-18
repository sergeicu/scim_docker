# We define equivalent list of dependencies from Simon's CRKit docker installation in centos format. 

# Simon's original docker is based on ubuntu. 
# Dependencies are listed for centos6
# Simon's docker file https://github.com/ComputationalRadiology/crkit/blob/main/Dockerfile

# Init centos6 docker image
nano docker_run_centos6.sh


# how to check if dependency exists 
yum search ncurses-devel

# how to install 
yum install -y ncurses-devel

# how to find a package that does not exist in 'yum search'
https://centos.pkgs.org/7/epel-x86_64/double-conversion-2.0.1-3.el7.x86_64.rpm.html
yum install wget
wget <binary_package> # search webpage for link 
rpm -Uvh <name_of_downloaded_package_rpm>
yum install <name_of_package>
<here is another example - https://www.cyberciti.biz/faq/installing-rhel-epel-repo-on-centos-redhat-7-x/> 





# LIST OF DEPENDENCIES (UBUNTU ON LEFT AND CENTOS ON RIGHT)
build-essential >> yum install gcc gcc-c++ make # (46M) https://unix.stackexchange.com/questions/16422/cant-install-build-essential-on-centos
                OR yum groupinstall 'Development Tools'   # (153Mb)
apt-utils >> yum update && yum install yum-utils
zlib1g-dev >>  yum install zlib-devel # ? 
libncurses5-dev >> yum install ncurses-devel
libgdbm-dev >>  gdbm-devel
* libnss3-dev >> nss-devel # version does not match
* libssl-dev >> openssl-devel # version does not match
libreadline-dev >> readline-devel
libffi-dev >> libffi-devel
* libxcb-xinerama0 libxcb-xinerama0-dev >>  libxcb-devel libXinerama # version does not match
wget >> same 
git  >> same 
perl >> same 
python3 >> yum install --nogpgcheck python36
python3-dev >> rh-python36-python-devel
python3-pip >> rh-python36-python-pip
-- libproj-dev >>  # not found - only available with centos7 - https://centos.pkgs.org/7/epel-x86_64/proj-4.8.0-4.el7.x86_64.rpm.html, 
                originally thought it was this - libprojectM-devel, but its incorrect. search more here -
                http://rpmfind.net/linux/RPM/mageia/cauldron/armv7hl/media/core/release/libproj-devel-7.2.1-1.mga9.armv7hl.html 
gperf >> gperf 
bison >> bison 
flex >> flex
unzip >> unzip
cmake >> cmake 
-- libgoogle-glog-dev >> # not found - only available with centos7 - https://centos.pkgs.org/7/epel-x86_64/glog-0.3.3-8.el7.x86_64.rpm.html
libgflags-dev >> gflags-devel
* libatlas-base-dev >>  atlas-devel # version does not match
libeigen3-dev >> eigen3-devel
libsuitesparse-dev >> suitesparse
* qt5-default >> qt5-qtbase-devel # version does not match - read more here - https://askubuntu.com/questions/1335184/qt5-default-not-in-ubuntu-21-04
libtbb-dev >> tbb-devel
* libfreetype6-dev >> freetype-devel # version does not match 
libfontconfig-dev >> fontconfig-devel
-- libdouble-conversion-dev # not found - only available with centos7 - https://centos.pkgs.org/7/epel-x86_64/double-conversion-2.0.1-3.el7.x86_64.rpm.html
liblz4-dev liblzma-dev >> lz4-devel xz-lzma-compat
libnetcdf-dev >> netcdf-devel
-- libnetcdf-cxx-legacy-dev # not found - only available with centos7 - https://centos.pkgs.org/7/epel-x86_64/netcdf-cxx-4.2-8.el7.x86_64.rpm.html
libogg-dev >> libogg-devel
libtheora-dev >> libtheora-devel
libpng-dev >> libpng-devel
* libjpeg-dev >> openjpeg-devel openjpeg-libs libjpeg-turbo-devel # version does not match 
libtiff-dev >> libtiff-devel
libjsoncpp-dev >> jsoncpp-devel
libexpat1-dev >> compat-expat1 
libglew-dev >> glew-devel
libhdf5-dev >> hdf5-devel
libqt5x11extras5-dev >> qt5-qtx11extras-devel
* libqt5opengl5-dev >> qt5-qtbase-gui # version does not match - found here - https://pkgs.org/download/libQt5OpenGL.so.5(Qt_5)
qtbase5-dev >> qt5-qtbase
qttools5-dev >> qt5-qttools
libxt-dev libxml2-dev >> libXt-devel   libxml2-devel
libsqlite3-dev >> sqlite-devel 
--libswscale-dev # not found - only available with centos7 - https://centos.pkgs.org/7/cheese-x86_64/libswscale-devel-2.6.2-2.x86_64.rpm.html






















##########################################################################################
# OLD
##########################################################################################

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

