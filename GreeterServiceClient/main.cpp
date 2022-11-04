// GreeterServiceClient.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#include <grpcpp/channel.h>
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include "grpcpp/client_context.h"
#include "my_async_worker.cpp"
#include <condition_variable>
#include "my_async_worker2..cpp"
using namespace std;

using mygreeterapp::GreeterMessage;
using mygreeterapp::GreeterService;

int main()
{
	std::string endpoint = "localhost:5138";

	grpc::ChannelArguments channelArgs;

	std::shared_ptr<Channel> channel = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
	

	GrpcWorker2 worker(channel);

	Status status = worker.Await();

	if (!status.ok()) {
		std::cout << "DEBUG:Rpc failed." << std::endl;
	}
	std::cout << "DEBUG:Exiting main." << std::endl;
	return 0;
}
