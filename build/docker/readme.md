# How to build image, test it and push to container registry

**Prerequisites**:
* Docker is installed on the machine(I recommend to install docker desktop)

**Steps**:
* Switch to the directory containing Dockerfile
* `$ docker login registry.gitlab.com`
* (use access token from gitlab as password, see the token in the access file in google docks)
* `$ docker build -t registry.gitlab.com/game-engine4894225/dory .`
  _(instead of "dory" at the very end of the image tag you can use your own image name)_
* `$ docker container run -it registry.gitlab.com/game-engine4894225/dory`
* Run some test build commands in the container
* `$ docker push registry.gitlab.com/game-engine4894225/dory`
* Use the image in the pipeline