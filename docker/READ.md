### Build docker 
```
name=scim:3T
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile .

```

