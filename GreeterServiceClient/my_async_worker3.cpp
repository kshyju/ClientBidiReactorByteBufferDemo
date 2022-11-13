#include "my_async_worker3.h"
#include "spdlog/spdlog.h"
#include <thread>
#include <iostream>
#include <memory>
#include <mutex>
#include <future>

AsyncWorker3::AsyncWorker3(std::shared_ptr<Channel> channel)
{
	std::unique_ptr<GreeterService::Stub> stub_(GreeterService::NewStub(channel));
	stub_.get()->async()->GreetStream(&client_context_, this);

	write_.set_message("Start");
	StartWrite(&write_);
	StartRead(&read_);
	StartCall();
}

void AsyncWorker3::OnWriteDone(bool ok)
{
	SPDLOG_INFO("OnWriteDone");		
}

void AsyncWorker3::OnWritesDoneDone(bool ok)
{
}

void AsyncWorker3::processMessage(GreeterMessage msg)
{
	SPDLOG_INFO("processMessage processing copy. {}", msg.message());
	auto ms = msg.message();
	GreeterMessage response;
	response.set_message("Client response for-" + ms);
	//write(response);
	SPDLOG_INFO("Writing client response {}", response.message());
	StartWrite(&response);
}

void AsyncWorker3::OnReadDone(bool ok)
{
	SPDLOG_INFO("OnReadDone. ok={}", ok);
	if (!ok) {
		SPDLOG_WARN("Failed to read response");
		return;
	}

	SPDLOG_DEBUG("OnReadDone OK");
	
	SPDLOG_INFO("New message received fro srvr. {}", read_.message());
	GreeterMessage copy(read_);
	auto f = std::async(std::launch::async, [this, &copy]() {
		processMessage(copy);
	});
	
	StartRead(&read_);;
}

void AsyncWorker3::OnDone(const grpc::Status& status)
{
	SPDLOG_DEBUG("OnDone. status.code={}, status.message={}", status.error_code(), status.error_message());

	if (status.ok()) {
		SPDLOG_DEBUG("Bi-directional stream ended. status.code={}, status.message={}", status.error_code(), status.error_message());
	}

	std::unique_lock<std::mutex> l(mu_);
	status_ = status;
	done_ = true;
	cv_.notify_one();
}



void AsyncWorker3::fireRead()
{
	//StartRead(&read_);
}

void AsyncWorker3::fireWrite()
{
	{
		absl::MutexLock lk(&writes_mtx_);
		if (writes_.empty()) {
			std::cout << "No message to write" << std::endl;
			return;
		}
	}
	std::cout << "fireWrite. Writing " << write_.DebugString() << std::endl;
	StartWrite(&write_);
}

void AsyncWorker3::fireClose()
{
}

void AsyncWorker3::write(GreeterMessage message)
{
	{
		absl::MutexLock lk(&writes_mtx_);
		writes_.push_back(message);
	}
	fireWrite();
}

Status AsyncWorker3::await()
{
	std::unique_lock<std::mutex> l(mu_);
	cv_.wait(l, [this] { return done_; });
	return std::move(status_);
}


