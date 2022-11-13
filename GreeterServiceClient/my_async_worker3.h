#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include "greetermessage.pb.h"
#include "greetermessage.grpc.pb.h"
#include <condition_variable>


#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using mygreeterapp::GreeterMessage;
using mygreeterapp::GreeterService;

using Channel = grpc::Channel;
using Status = grpc::Status;

class AsyncWorker3: public grpc::ClientBidiReactor<GreeterMessage, GreeterMessage>
{
public:
	AsyncWorker3(std::shared_ptr<Channel> channel);

	void OnWriteDone(bool ok) override;

	void OnWritesDoneDone(bool ok) override;

	void OnReadDone(bool ok) override;

	void OnDone(const grpc::Status& status) override;

	void processMessage(GreeterMessage message);

	void fireRead();

	void fireWrite();

	void fireClose();

	void write(GreeterMessage message);

	Status await();

private:

	std::mutex mu_;
	std::condition_variable cv_;
	Status status_;
	bool done_ = false;


	grpc::ClientContext client_context_;
	/// <summary>
	/// Message to read from server.
	/// </summary>
	GreeterMessage read_;

	/// <summary>
	/// Message to write to server.
	/// </summary>
	GreeterMessage write_;

	/**
 * @brief Buffered commands to write to server
 */
	std::vector<GreeterMessage> writes_ GUARDED_BY(writes_mtx_);
	absl::Mutex writes_mtx_;

};