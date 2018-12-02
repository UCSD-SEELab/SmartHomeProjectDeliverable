docker-apollomq
==============

This project builds a [docker](http://docker.io/) container for running Apache (ActiveMQ) Apollo message broker. STOMP, AMQP, MQTT, Openwire, SSL, and WebSockets are supported.


## Build

Once you have [installed docker](https://www.docker.io/gettingstarted/#h_installation) you should be able to create the containers via the following:

```
git clone https://github.com/pires/docker-apollomq.git
cd docker-apollomq
docker build -t apollomq:1.7.1 --build-arg ADMIN_PASSWORD=<yout_password> .
```

## Run

Non-secure ports:

```
docker run --name amq1 -p=61613:61613 -p=61623:61623 -p=61680:61680 -d -t apollomq:1.7.1
```

Secure ports:

```
docker run --name amq1 -p=61614:61614 -p=61624:61624 -p=61681:61681 -d -t apollomq:1.7.1
```

## Test
In our SamrtHome tests we built the docker image with the command
```
docker build -t apollomq:1.7.1 --build-arg ADMIN_PASSWORD=SamsungProject .
```
and we run it on non-secure prots (to simplify the deployment on embedded hardware with no hardware acceleration for encryption.

You can access the message broker console form the browser on the pc running docker at address (http://localhost:61680)
```
login: admin
passcode: SamsungProject
```
Your configuration for connecting to MQTT via TCP are:
```
username: admin
password: SamsungProject
address: <The IP of the server running docker>
port: 62613
```
Connect as you see fit.
