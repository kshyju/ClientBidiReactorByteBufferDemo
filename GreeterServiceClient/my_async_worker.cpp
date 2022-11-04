#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include "greetermessage.pb.h"
#include "greetermessage.grpc.pb.h"
#include <condition_variable>

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using mygreeterapp::GreeterMessage;
using mygreeterapp::GreeterService;

class GrpcWorker1 : public grpc::ClientBidiReactor<GreeterMessage, GreeterMessage> {
public:
	explicit GrpcWorker1(std::shared_ptr<Channel> channel) {

		std::unique_ptr<GreeterService::Stub> stub_(GreeterService::NewStub(channel));
		stub_.get()->async()->GreetStream(&context_, this);

		GreeterMessage* msg = new GreeterMessage();
		msg->set_message("Start");
		StartWrite(msg);

		StartRead(&messageFromServer_);
		StartCall();
	}

	void OnWriteDone(bool ok) override {
		std::cout << "DEBUG:OnWriteDone:" << ok << std::endl;
	}

	void OnReadDone(bool ok) override {
		std::cout << "DEBUG:OnReadDone:" << ok << std::endl;
		if (ok) {
			
			std::cout << "DEBUG:message:" << messageFromServer_.message() << std::endl;

			if (messageFromServer_.message() == "InitRequest")
			{
				std::unique_ptr<GreeterMessage> msg1 = std::make_unique<GreeterMessage>();
				msg1->set_message("InitResponse");
				StartWrite(msg1.get());
			}
			else if (messageFromServer_.message() == "MetaRequest")
			{
				std::unique_ptr<GreeterMessage> msg = std::make_unique<GreeterMessage>();
				msg->set_message("MetaResponse");
				StartWrite(msg.get());
			}
			else
			{
			}
			StartRead(&messageFromServer_);
		}
		// if false, we should Ideally explicitly close by calling Finish?
	}
	Status Await() {
		std::unique_lock<std::mutex> l(mu_);
		cv_.wait(l, [this] { return done_; });
		return std::move(status_);
	}

private:
	ClientContext context_;

	// Every single read will use this instance.
	//  There can only be one outstanding read or write at a given time on a given
	//  stream, but reads and writes can happen in parallel.
	//  https://groups.google.com/g/grpc-io/c/T2x2kVxnPWk/m/ane8aRJIAwAJ
	mygreeterapp::GreeterMessage messageFromServer_;
	std::mutex mu_;
	std::condition_variable cv_;
	Status status_;
	bool done_ = false;
};