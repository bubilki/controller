#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <chrono>

#include "mqtt/client.h"


std::string generate_uuid_v4();
const std::string SERVER = "hawk.rmq.cloudamqp.com:1883"; //Broker address
const std::string CLIENT_ID = generate_uuid_v4();
const std::string TOPIC = "sensors_data/" + CLIENT_ID;
const std::string PASSWORD = "U4p0L3VsKEd_P0uZP57x3k81H3YO3b0V";
const std::string USERNAME = "pzqsfkxt:pzqsfkxt";
const int QoS = 1;

std::random_device rd{};
std::mt19937 gen{rd()};
std::normal_distribution<float>* distribution;
float watt_usage;

std::string generate_uuid_v4() {
	std::uniform_int_distribution<> dis(0, 15);
	std::uniform_int_distribution<> dis2(8, 11);
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    std::default_random_engine generator(static_cast<uint64_t>(ms.count()));

	std::stringstream ss;
	int i;
	ss << std::hex;
	for (i = 0; i < 8; i++) {
		ss << dis(generator);
	}
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << dis(generator);
	}
	ss << "-4";
	for (i = 0; i < 3; i++) {
		ss << dis(generator);
	}
	ss << "-";
	ss << dis2(gen);
	for (i = 0; i < 3; i++) {
		ss << dis(generator);
	}
	ss << "-";
	for (i = 0; i < 12; i++) {
		ss << dis(generator);
	};
	return ss.str();
}

std::string form_json(const std::string& name, float value)
{
	std::string json;
	json = "{\"name\":\"" + name + "\" , \"value\":" + "\"" + std::to_string(value) + "\", \"uuid\":\"" + CLIENT_ID + "\"}";
	return json;
}

void sensor_process(const std::string& name, mqtt::client& client)
{
    distribution = new std::normal_distribution<float>{0, 0.3};
    watt_usage += (*distribution)(gen);

	std::string payload = form_json(name,  watt_usage);
	auto pubmsg = mqtt::make_message(TOPIC, payload);
	pubmsg->set_qos(QoS);
	client.publish(pubmsg);
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << " sent to "<<SERVER<<std::endl;
	std::cout<<pubmsg.get()->to_string()<<std::endl;
    bool ok = false;
    mqtt::const_message_ptr msg;
    try {
        ok = client.try_consume_message(&msg);
    } catch (...) {}
    if(ok)
    {
        if(msg->to_string() == "{\"msg\":\"off\"}")
        {
            std::cout << "Turn off message recieved"<<std::endl;
            exit(0);
        }
    }
	sleep(5);
}

int main(int argc, char* argv[])
{
    std::string name = std::getenv("SENSOR_NAME") ? std::getenv("SENSOR_NAME") : "";

    if(name.empty())
    {
        if(argc != 2)
        {
            std::cerr << "Please pass the name of the device in as environmental variable";
            exit(1);
        }

        name = argv[1];
    }

    distribution = new std::normal_distribution<float>{120.0, 4};
    watt_usage = (*distribution)(gen);

	try {
        std::cout<<"sensor_name: "<<name<<std::endl;
        std::cout<<"uuid: "<<CLIENT_ID<<std::endl;
		mqtt::client client(SERVER, CLIENT_ID);
		mqtt::connect_options connOpts;
		connOpts.set_keep_alive_interval(20);
		connOpts.set_clean_session(true);
		connOpts.set_password(PASSWORD);
		connOpts.set_user_name(USERNAME);

		client.connect(connOpts);
        client.subscribe("off/" + CLIENT_ID, 0);

		while (client.is_connected()) {
			sensor_process(name, client);
		}
	}
	catch (const mqtt::exception& e)
	{
		std::cerr<<e.what();
	}

	return 0;
}