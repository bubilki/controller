#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>

#include "mqtt/client.h"


std::string generate_uuid_v4();
const std::string SERVER = "hawk.rmq.cloudamqp.com"; //Broker address
const std::string CLIENT_ID = generate_uuid_v4();
const std::string TOPIC = "sensors_data";
const std::string PASSWORD = "hdj973ZX7qrwvZki6_gHAPbP6PZf5eQV";
const std::string USERNAME = "desizaxr:desizaxr";
const int QoS = 1;

std::random_device rd{};
std::mt19937 gen{rd()};
std::normal_distribution<float>* distribution;

std::string generate_uuid_v4() {
	std::uniform_int_distribution<> dis(0, 15);
	std::uniform_int_distribution<> dis2(8, 11);

	std::stringstream ss;
	int i;
	ss << std::hex;
	for (i = 0; i < 8; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << dis(gen);
	}
	ss << "-4";
	for (i = 0; i < 3; i++) {
		ss << dis(gen);
	}
	ss << "-";
	ss << dis2(gen);
	for (i = 0; i < 3; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 12; i++) {
		ss << dis(gen);
	};
	return ss.str();
}

std::string form_json(const std::string& name, const std::string& type, float value)
{
	std::string json;
	json = "{\"name\": \"" + name + "\", \"type\":\"" + type + "\", \"value\":" + std::to_string(value) + "}";
	return json;
}

void sensor_process(const std::string& name, const std::string& type, mqtt::client& client)
{
	if(type == "smoke")
	{
		distribution = new std::normal_distribution<float>{0.5, 0.15};
	}
	else if(type == "temp")
	{
		distribution = new std::normal_distribution<float>{21.0, 4.0};
	}
	else if(type == "oxygen")
	{
		distribution = new std::normal_distribution<float>{19.5, 4};
	}
	else
	{
		std::cerr<<"Sensor of non existing type";
		exit(2);
	}
	std::string payload = form_json(name, type, (*distribution)(gen));
	auto pubmsg = mqtt::make_message(TOPIC, payload);
	pubmsg->set_qos(QoS);
	client.publish(pubmsg);
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << " sent to "<<SERVER<<std::endl;
	std::cout<<pubmsg.get()->to_string()<<std::endl;
	sleep(5);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cerr << "Please pass the name and the type of the sensor in command line arguments";
		exit(1);
	}

	std::string name(argv[1]);
	std::string type(argv[2]);
	try {
		mqtt::client client(SERVER, CLIENT_ID);
		mqtt::connect_options connOpts;
		connOpts.set_keep_alive_interval(20);
		connOpts.set_clean_session(true);
		connOpts.set_password(PASSWORD);
		connOpts.set_user_name(USERNAME);

		client.connect(connOpts);

		while (client.is_connected()) {
			sensor_process(name, type, client);
		}
	}
	catch (const mqtt::exception& e)
	{
		std::cerr<<e.what();
	}

	return 0;
}