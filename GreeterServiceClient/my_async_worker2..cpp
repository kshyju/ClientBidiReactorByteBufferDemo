#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/generic/generic_stub.h>
#include <grpc/byte_buffer.h>
#include "greetermessage.pb.h"
#include "greetermessage.grpc.pb.h"
#include <condition_variable>
#include <absl/memory/memory.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
using namespace grpc;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using mygreeterapp::GreeterMessage;
using mygreeterapp::GreeterService;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ByteBuffer;

class GrpcWorker2 : public grpc::ClientBidiReactor<ByteBuffer, ByteBuffer> {
public:
	explicit GrpcWorker2(std::shared_ptr<Channel> channel) {

		generic_stub_ = absl::make_unique<GenericStub>(channel);

		const char* suffix_for_stats = nullptr;
		cli_ctx_ = absl::make_unique<ClientContext>();
		StubOptions options(suffix_for_stats);
		generic_stub_->PrepareBidiStreamingCall(
			cli_ctx_.get(), "GreetStream", options, this);
			
		request_.set_message("test_str");
		send_buf_ = SerializeToByteBuffer(&request_);
		StartWrite(send_buf_.get());

		StartRead(&readMessageByteBuffer_);
		StartCall();
	}

	void OnWriteDone(bool ok) override {
		std::cout << "DEBUG:OnWriteDone:" << ok << std::endl;
	}

	void OnReadDone(bool ok) override {
		std::cout << "DEBUG:OnReadDone:" << ok << std::endl;
		if (ok) {
			StartRead(&readMessageByteBuffer_);
		}
	}
	Status Await() {
		std::unique_lock<std::mutex> l(mu_);
		cv_.wait(l, [this] { return done_; });
		return std::move(status_);
	}

	std::unique_ptr<ByteBuffer> SerializeToByteBuffer(
		grpc::protobuf::Message* message) {
		std::string buf;
		message->SerializeToString(&buf);
		Slice slice(buf);
		return absl::make_unique<ByteBuffer>(&slice, 1);
	}

private:
	std::unique_ptr<grpc::GenericStub> generic_stub_;
	std::unique_ptr<ClientContext> cli_ctx_;
	grpc::ByteBuffer readMessageByteBuffer_;
	GreeterMessage request_;
	ByteBuffer recv_buf_;
	std::unique_ptr<ByteBuffer> send_buf_;
	std::mutex mu_;
	std::condition_variable cv_;
	Status status_;
	bool done_ = false;

};