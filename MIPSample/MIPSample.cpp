// MIPSample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "mip/mip_context.h"  
#include "auth_delegate.h"
#include "consent_delegate.h"
#include "profile_observer.h"

using std::promise;
using std::future;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::cout;
using mip::ApplicationInfo;
using mip::FileProfile;
using mip::FileEngine;
using std::endl;
#include "filehandler_observer.h" 
#include "mip/file/file_handler.h"
using mip::FileHandler;

int main()
{

    // Construct/initialize objects required by the application's profile object
    // ApplicationInfo object (App ID, name, version)
    ApplicationInfo appInfo{ "d40ea3e1-d9cb-4082-921f-b0b8d2da2402",
                            "MIP File SDK Sample AppCplusPlus",
                            "1.11" };

    // Create MipConfiguration object.
    std::shared_ptr<mip::MipConfiguration> mipConfiguration = std::make_shared<mip::MipConfiguration>(appInfo,
        "mip_data",
        mip::LogLevel::Trace,
        false);


    std::shared_ptr<mip::MipContext> mMipContext = mip::MipContext::Create(mipConfiguration);

    auto profileObserver = make_shared<ProfileObserver>();                     // Observer object
    auto authDelegateImpl = make_shared<AuthDelegateImpl>("d40ea3e1-d9cb-4082-921f-b0b8d2da2402"); // Authentication delegate object (App ID)                 
    auto consentDelegateImpl = make_shared<ConsentDelegateImpl>();             // Consent delegate object

    // Construct/initialize profile object
    FileProfile::Settings profileSettings(
        mMipContext,
        mip::CacheStorageType::InMemory,
        consentDelegateImpl,
        profileObserver);

    // Set up promise/future connection for async profile operations; load profile asynchronously
    auto profilePromise = make_shared<promise<shared_ptr<FileProfile>>>();
    auto profileFuture = profilePromise->get_future();

    try
    {
        mip::FileProfile::LoadAsync(profileSettings, profilePromise);
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... are the Settings and ApplicationInfo objects populated correctly?\n\n" << e.what() << "'\n";

        system("pause");
        return 1;
    }
    auto profile = profileFuture.get();

    // Construct/initialize engine object
    FileEngine::Settings engineSettings(
        mip::Identity("pramkum@pramkumlab.onmicrosoft.com"), // Engine identity (account used for authentication)
        authDelegateImpl,		       // Token acquisition implementation
        "My App State",                  // User-defined engine state
        "en-US");                          // Locale (default = en-US)

    // Set the engineId for caching. 
    engineSettings.SetEngineId("pramkum@pramkumlab.onmicrosoft.com");
    // Set up promise/future connection for async engine operations; add engine to profile asynchronously
    auto enginePromise = make_shared<promise<shared_ptr<FileEngine>>>();
    auto engineFuture = enginePromise->get_future();
    profile->AddEngineAsync(engineSettings, enginePromise);
    std::shared_ptr<FileEngine> engine;
    try
    {
        engine = engineFuture.get();
        // List sensitivity labels
        cout << "\nSensitivity labels for your organization:\n";
        auto labels = engine->ListSensitivityLabels();
        for (const auto& label : labels)
        {
            cout << label->GetName() << " : " << label->GetId() << endl;

            for (const auto& child : label->GetChildren())
            {
                cout << "->  " << child->GetName() << " : " << child->GetId() << endl;
            }
        }
        system("pause");
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... is the access token incorrect/expired?\n\n" << e.what() << "'\n";

        system("pause");
        return 1;
    }
    // Set up async FileHandler for input file operations
    string inputFilePath = "C:\\O\\MIP\\TestDoc.docx";
    string actualFilePath = "C:\\O\\MIP\\TestDoc.docx";
    string filePathIn= "C:\\O\\MIP\\TestDoc.docx";


    std::shared_ptr<FileHandler> handler;
    try
    {
        auto handlerPromise = std::make_shared<std::promise<std::shared_ptr<FileHandler>>>();
        auto handlerFuture = handlerPromise->get_future();
        engine->CreateFileHandlerAsync(
            inputFilePath,
            actualFilePath,
            true,
            std::make_shared<FileHandlerObserver>(),
            handlerPromise);
        handler = handlerFuture.get();
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... did you specify a valid input file path?\n\n" << e.what() << "'\n";
        system("pause");
        return 1;
    }

    // Set a label on input file
    try
    {
        string labelId = "4abd68ad-a6e9-4a43-b163-23fabb45da02";
        cout << "\nApplying Label ID " << labelId << " to " << filePathIn << endl;
        mip::LabelingOptions labelingOptions(mip::AssignmentMethod::STANDARD);
        handler->SetLabel(engine->GetLabelById(labelId), labelingOptions, mip::ProtectionSettings());
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... did you specify a valid label ID?\n\n" << e.what() << "'\n";
        system("pause");
        return 1;
    }

    // Commit changes, save as a different/output file
    string filePathOut = "C:\\O\\MIP\\TestDoc_Cpp.docx";
    try
    {
        cout << "Committing changes" << endl;
        auto commitPromise = std::make_shared<std::promise<bool>>();
        auto commitFuture = commitPromise->get_future();
        handler->CommitAsync(filePathOut, commitPromise);
        if (commitFuture.get()) {
            cout << "\nLabel committed to file: " << filePathOut << endl;
        }
        else {
            cout << "Failed to label: " + filePathOut << endl;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... did you specify a valid commit file path?\n\n" << e.what() << "'\n";
        system("pause");
        return 1;
    }
    system("pause");

    // Set up async FileHandler for output file operations
    actualFilePath = "C:\\O\\MIP\\TestDoc_Cpp.docx";
    try
    {
        auto handlerPromise = std::make_shared<std::promise<std::shared_ptr<FileHandler>>>();
        auto handlerFuture = handlerPromise->get_future();
        engine->CreateFileHandlerAsync(
            filePathOut,
            actualFilePath,
            true,
            std::make_shared<FileHandlerObserver>(),
            handlerPromise);

        handler = handlerFuture.get();
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... did you specify a valid output file path?\n\n" << e.what() << "'\n";
        system("pause");
        return 1;
    }

    // Get the label from output file
    try
    {
        cout << "\nGetting the label committed to file: " << filePathOut << endl;
        auto label = handler->GetLabel();
        cout << "Name: " + label->GetLabel()->GetName() << endl;
        cout << "Id: " + label->GetLabel()->GetId() << endl;
    }
    catch (const std::exception& e)
    {
        cout << "An exception occurred... did you specify a valid label ID?\n\n" << e.what() << "'\n";
        system("pause");
        return 1;
    }
    system("pause");


    // Application shutdown. Null out profile and engine, call ReleaseAllResources();
    // Application may crash at shutdown if resources aren't properly released.
    // handler = nullptr; // This will be used in later quick starts.
    engine = nullptr;
    profile = nullptr;
    handler = nullptr;
    mMipContext->ShutDown();
    mMipContext = nullptr;

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
