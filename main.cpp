#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <regex>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits>
#include <cstring>

#include <curl/curl.h>

using namespace std;

std::string findDevice() {
    const std::string devPath = "/dev/";
    const std::string prefix = "ttyACM";
    // const std::string prefix = "ttyUSB";
    std::string smallestDevice = "";
    int smallestNumber = 0;

    cout << "Serching device " << devPath + prefix << " ... " << endl;

    while (true){
        DIR* dir = opendir(devPath.c_str());
        if (!dir) {
            std::cerr << "Failed to open /dev directory: " << strerror(errno) << std::endl;
            return "";
        }

        smallestNumber = std::numeric_limits<int>::max();

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fileName(entry->d_name);
            if (fileName.rfind(prefix, 0) == 0) { // Check if it starts with "ttyACM"
                std::string numberStr = fileName.substr(prefix.length());
                if (!numberStr.empty() && std::all_of(numberStr.begin(), numberStr.end(), ::isdigit)) {
                    int number = std::stoi(numberStr);
                    if (number < smallestNumber) {
                        smallestNumber = number;
                        smallestDevice = devPath + fileName;
                    }
                }
            }
        }
        closedir(dir);
        if (smallestDevice.length() > 0 ) break;
        sleep(1);
    }
    cout << "Scanner found on port : " << smallestDevice << endl;
    return smallestDevice;
}

bool deviceExists(const char* device) {
    struct stat buffer;
    return (stat(device, &buffer) == 0);
}

int openDevice(const char* device) {
    return open(device, O_RDONLY | O_NOCTTY | O_NONBLOCK);
}

void sendHttpPost(const std::string& barcode) {
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "http://10.0.10.64:1880/slicer4/barcode");

        // Prepare JSON payload
        std::string jsonPayload = "{\"value\": \"" + barcode + "\"}";

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        // Set HTTP headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "HTTP POST failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // std::cout << "HTTP POST succeeded with payload: " << jsonPayload << std::endl;
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}

bool configureScannerPort(int serialPort){
    struct termios tty;
    if (tcgetattr(serialPort, &tty) != 0) {
        perror("Error getting terminal attributes");
        close(serialPort);
        return false;
    }

    // Set baud rate
    cfsetispeed(&tty, B57600);
    cfsetospeed(&tty, B57600);

    // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    tty.c_cflag &= ~PARENB;  // No parity
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit
    tty.c_cflag &= ~CSIZE;   
    tty.c_cflag |= CS8;      // 8 data bits

    // Disable flow control
    tty.c_cflag &= ~CRTSCTS;

    // Disable canonical mode, echo, and signal interpretation
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Set raw output mode
    tty.c_oflag &= ~OPOST;

    // Set read timeout: 1 decisecond (100 ms)
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;

    // Apply settings
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        perror("Error setting terminal attributes");
        close(serialPort);
        return false;
    }
    return true;
}

int main() {
    bool deviceOk = false;
    string scannerDev = "";
    int serialPort = 0;
    char buffer[256];

    // Read and print data
    while (true) {
        
        if(scannerDev.length() <= 0){
            scannerDev = findDevice();
            deviceOk = false;
        }

        if(!deviceExists(scannerDev.c_str())){
            close(serialPort);
            scannerDev = findDevice();
            deviceOk = false;
        }

        if(!deviceOk){
            serialPort = openDevice(scannerDev.c_str());
            if(serialPort >= 0){
                cout << "Port " << scannerDev << " opened : " << serialPort << endl;
                if( configureScannerPort(serialPort) ){
                    cout << "Port configured" << endl;
                    deviceOk = true;
                }
            }
        }

        if(deviceOk){
            ssize_t bytesRead = read(serialPort, buffer, sizeof(buffer) - 1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // Null-terminate the string

                std::string barcode(buffer);
                
                // Send HTTP POST with the barcode
                sendHttpPost(barcode);
                cout << "Barcode : " << barcode << endl;
            }
        }
    }

    // Close the serial port (unreachable in this example)
    close(serialPort);

    return EXIT_SUCCESS;
}
