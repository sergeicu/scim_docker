name=scim:latest
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile_copy_binaries .

# how to break the build process: 
https://stackoverflow.com/questions/35154219/rebuild-docker-image-from-specific-step

# how to remove dangling images: 
https://dzone.com/articles/docker-layers-explained

# 