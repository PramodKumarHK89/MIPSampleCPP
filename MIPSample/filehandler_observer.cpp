#include "filehandler_observer.h"

void FileHandlerObserver::OnCreateFileHandlerSuccess(const std::shared_ptr<mip::FileHandler>& fileHandler, const std::shared_ptr<void>& context) {
	auto promise = std::static_pointer_cast<std::promise<std::shared_ptr<mip::FileHandler>>>(context);
	promise->set_value(fileHandler);
}

void FileHandlerObserver::OnCreateFileHandlerFailure(const std::exception_ptr& error, const std::shared_ptr<void>& context) {
	auto promise = std::static_pointer_cast<std::promise<std::shared_ptr<mip::FileHandler>>>(context);
	promise->set_exception(error);
}

void FileHandlerObserver::OnCommitSuccess(bool committed, const std::shared_ptr<void>& context) {
	auto promise = std::static_pointer_cast<std::promise<bool>>(context);
	promise->set_value(committed);
}

void FileHandlerObserver::OnCommitFailure(const std::exception_ptr& error, const std::shared_ptr<void>& context) {
	auto promise = std::static_pointer_cast<std::promise<bool>>(context);
	promise->set_exception(error);
}
