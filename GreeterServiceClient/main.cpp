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
#include "my_async_worker3.h"
using namespace std;

using mygreeterapp::GreeterMessage;
using mygreeterapp::GreeterService;

int main()
{
	std::string endpoint = "localhost:5138";

	grpc::ChannelArguments args;

	//// Use unlimited receive message size.
	//channel_arguments_.SetMaxReceiveMessageSize(-1);

	//int max_send_message_size = 1024 * 1024 * 16;
	//channel_arguments_.SetMaxSendMessageSize(max_send_message_size);
	args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
	args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 30000);
	args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 10000);
	args.SetInt(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);

	/*
	 * Keep-alive settings:
	 * https://github.com/grpc/grpc/blob/master/doc/keepalive.md
	 * Keep-alive ping timeout duration: 3s
	 * Keep-alive ping interval, 30s
	 */
	//channel_arguments_.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 60000);
	//channel_arguments_.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 3000);
	//channel_arguments_.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
	//channel_arguments_.SetInt(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);

	/*
	 * If set to zero, disables retry behavior. Otherwise, transparent retries
	 * are enabled for all RPCs, and configurable retries are enabled when they
	 * are configured via the service config. For details, see:
	 *   https://github.com/grpc/proposal/blob/master/A6-client-retries.md
	 */
	//channel_arguments_.SetInt(GRPC_ARG_ENABLE_RETRIES, 0);

	//channel_arguments_.SetSslTargetNameOverride("localhost");

	std::shared_ptr<Channel> channel = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
	

	AsyncWorker3 worker(channel);

	Status status = worker.await();

	if (!status.ok()) {
		std::cout << "DEBUG:Rpc failed." << std::endl;
	}
	std::cout << "DEBUG:Exiting main." << std::endl;
	return 0;
}
