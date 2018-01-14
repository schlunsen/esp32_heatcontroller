Requirements
-------------

* MQTT server - I use [mosquitto](https://mosquitto.org/)
* supervisor
* virtualenv


Installation
-------------
```
adduser heat

sudo su - heat

virtualenv env

pip install -r requirements.txt

# Install supervisor file
cp supervisor/heat_controller.conf /etc/supervisor/conf.d

supervisorctl update

```