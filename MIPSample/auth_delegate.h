#include <string>
#include "mip/common_types.h"

class AuthDelegateImpl final : public mip::AuthDelegate {
public:
    AuthDelegateImpl() = delete;        // Prevents default constructor

    AuthDelegateImpl(
        const std::string& appId)         // AppID for registered AAD app
        : mAppId(appId) {};

    bool AcquireOAuth2Token(            // Called by MIP SDK to get a token
        const mip::Identity& identity,    // Identity of the account to be authenticated, if known
        const OAuth2Challenge& challenge, // Authority (AAD tenant issuing token), and resource (API being accessed; "aud" claim).
        OAuth2Token& token) override;     // Token handed back to MIP SDK

private:
    std::string mAppId;
    std::string mToken;
    std::string mToken2;
    std::string mAuthority;
    std::string mResource;
};