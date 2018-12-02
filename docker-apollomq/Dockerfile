# You know you love it
FROM openjdk:8-slim
ARG ADMIN_PASSWORD
# Me, Myself and I
# Original author: Paulo Pires <pjpires@gmail.com>
#Custumized to redcue the size and make the password configurable by Massimiliano Menarini <mmeanrini@ucsd.edu>
MAINTAINER Massimiliano Menarini <mmeanrini@ucsd.edu>

RUN apt-get update && \
    apt-get upgrade -y && \
	apt-get install -y sed wget && \
	apt-get autoclean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers && \
    sed -i 's/.*requiretty$/#Defaults requiretty/' /etc/sudoers && \
	adduser --disabled-password --gecos '' apollo && \
	adduser apollo sudo && \
	echo "export JAVA_HOME=/usr/lib/jvm/jre" >> /etc/profile && \
	echo "alias ll='ls -l --color=auto'" >> /etc/profile && \
	echo "alias grep='grep --color=auto'" >> /etc/profile

# Download, extract and install Apollo MQ
WORKDIR /home/apollo
USER apollo
RUN wget -c http://www.eu.apache.org/dist/activemq/activemq-apollo/1.7.1/apache-apollo-1.7.1-unix-distro.tar.gz && \
    tar -zxvf apache-apollo-1.7.1-unix-distro.tar.gz && \
    rm apache-apollo-1.7.1-unix-distro.tar.gz
RUN apache-apollo-1.7.1/bin/apollo create mybroker && \
	sed -i -e 's/password/'"$ADMIN_PASSWORD"'/g' mybroker/etc/users.properties && \
	sed -i -e 's|/127.0.0.1|/0.0.0.0|g' mybroker/etc/apollo.xml 
WORKDIR /home/apollo/mybroker
# TODO remove unsecured endpoints
EXPOSE 61613 61614 61623 61624 61680 61681

CMD  bin/apollo-broker run
