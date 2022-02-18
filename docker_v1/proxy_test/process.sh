name=proxy
docker build --no-cache  --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t ${name}_remove -f Dockerfile . 