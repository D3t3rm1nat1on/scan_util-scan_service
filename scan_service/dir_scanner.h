#ifndef SCAN_SERVICE_DIR_SCANNER_H
#define SCAN_SERVICE_DIR_SCANNER_H

#include <string>
#include <filesystem>
#include <thread>
#include <vector>
#include <fstream>

#include "../include/scanner_defines.h"

int scan_directory(const char *dir, scan_results &results);

#endif