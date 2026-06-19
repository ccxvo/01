#pragma once

#include <cstdint>

namespace server {

// Start the HTTP server on the specified port.
// This function blocks indefinitely (accept loop).
// Returns 0 on success, non-zero on error.
int run(int port = 8080);

} // namespace server
