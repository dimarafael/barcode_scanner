#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <curl/curl.h>

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
            std::cout << "HTTP POST succeeded with payload: " << jsonPayload << std::endl;
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
}


int main() {
    const char* device = "/dev/ttyACM0";
    int serialPort = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serialPort == -1) {
        perror("Error opening serial port");
        return 1;
    }

    char buffer[256];
    while (true) {
        int bytesRead = read(serialPort, buffer, sizeof(buffer) - 1);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string barcode(buffer);

            std::cout << "Scanned barcode: " << buffer << std::endl;

            // Send HTTP POST with the barcode
            sendHttpPost(barcode);
        }
    }

    close(serialPort);
    return 0;
}
