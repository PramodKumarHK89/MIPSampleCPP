#include "consent_delegate.h"
#include <iostream>
using mip::Consent;
using std::string;

Consent ConsentDelegateImpl::GetUserConsent(const string& url)
{
    // Accept the consent to connect to the url
    std::cout << "SDK will connect to: " << url << std::endl;
    return Consent::AcceptAlways;
}