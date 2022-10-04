#ifdef LAVENDER_WIN32_SUPPORT_ENABLED

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <sstream>

#include "Source/Base/Error.hpp"

namespace platform::win32 {

    HINSTANCE g_hInstance{nullptr};

} // namespace platform::win32

void
base::Error::displayErrorMessageBox() const noexcept {
    if (m_success)
        return;

    std::stringstream stream;
    stream << "An error occurred in " << libraryName() << " in " << className()
           << "\n"
           << "\nDescription: " << description()
           << "\nAttempt: " << attemptedAction()
           << "\n"
           << "\nFile: " << sourceLocation().file_name() << ':' << sourceLocation().line() 
           << "\nFunction: " << sourceLocation().function_name()
    ;

    auto message = stream.str();
    MessageBox(nullptr, message.c_str(), "", MB_OK | MB_ICONHAND | MB_SETFOREGROUND);
}

#endif // LAVENDER_WIN32_SUPPORT_ENABLED
