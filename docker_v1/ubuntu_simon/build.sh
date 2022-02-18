name=ubuntu_simon
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name .
docker run --rm -it ubuntu_simon /bin/bash



