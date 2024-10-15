#include "mip/common_types.h"
#include <string>

class ConsentDelegateImpl final : public mip::ConsentDelegate {
public:
    ConsentDelegateImpl() = default;
    virtual mip::Consent GetUserConsent(const std::string& url) override;
};