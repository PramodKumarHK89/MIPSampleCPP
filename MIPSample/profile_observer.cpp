#include "profile_observer.h"
#include <future>

using std::promise;
using std::shared_ptr;
using std::static_pointer_cast;
using mip::FileEngine;
using mip::FileProfile;

void ProfileObserver::OnLoadSuccess(const shared_ptr<FileProfile>& profile, const shared_ptr<void>& context) {
    auto promise = static_pointer_cast<std::promise<shared_ptr<FileProfile>>>(context);
    promise->set_value(profile);
}

void ProfileObserver::OnLoadFailure(const std::exception_ptr& error, const shared_ptr<void>& context) {
    auto promise = static_pointer_cast<std::promise<shared_ptr<FileProfile>>>(context);
    promise->set_exception(error);
}

void ProfileObserver::OnAddEngineSuccess(const shared_ptr<FileEngine>& engine, const shared_ptr<void>& context) {
    auto promise = static_pointer_cast<std::promise<shared_ptr<FileEngine>>>(context);
    promise->set_value(engine);
}

void ProfileObserver::OnAddEngineFailure(const std::exception_ptr& error, const shared_ptr<void>& context) {
    auto promise = static_pointer_cast<std::promise<shared_ptr<FileEngine>>>(context);
    promise->set_exception(error);
}
