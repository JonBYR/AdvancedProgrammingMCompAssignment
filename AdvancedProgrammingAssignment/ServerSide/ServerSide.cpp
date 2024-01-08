#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link the Winsock library

#include <iostream>
#include "imageProcessing.h"
#include "rotation.h"
#include "InvalidOperationException.h"
#include "NonNumeric.h"
#include "resize.h"
#include "ColourAdjustment.h"
#include "Blur.h"
#include "Flip.h"
#include "ColourConvert.h"
#include <opencv2/opencv.hpp>
#include "MissingArgs.h"

#include <cstring>
#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#define BUFFER_SIZE 1024 //buffer for UDP packets
using namespace std;
enum Filters { ROTATE, COLOURADJUST, BLUR, FLIP, RESIZE, COLOURCONVERT }; //enum used to contain filters
Filters enumConvert(const string& filterName) throw(InvalidOperationException) //const used to make sure filterName does not change
{
    if (filterName == "rotate") return ROTATE;
    else if (filterName == "brightness" || filterName == "contrast" || filterName == "gamma") return COLOURADJUST;
    else if (filterName == "guassian" || filterName == "box" || filterName == "sharpening") return BLUR;
    else if (filterName == "flip") return FLIP;
    else if (filterName == "size") return RESIZE;
    else if (filterName == "greyscale" || filterName == "hsv") return COLOURCONVERT;
    else {
        throw InvalidOperationException(filterName);
    }
}
void lowerCase(string& s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {return tolower(c); }); //standard transform function in c++
}
void sendImage(int socket, cv::Mat image, sockaddr_in server, const string& path)
{
    vector<uchar> imageToSend;
    cv::imencode(path, image, imageToSend);
    size_t imageSize = imageToSend.size();
    char* imageBuffer = new char[imageSize];
    char* currentSendPos = &imageBuffer[0]; //starting position to send image data 
    size_t remainingBytes = imageSize; //size of image left to send (that is partitioned in packets)
    char sizeBuffer[sizeof(size_t)];
    memcpy(sizeBuffer, &imageSize, sizeof(size_t)); //stores the size of the image as a char
    sendto(socket, (const char*)sizeBuffer, sizeof(size_t), 0, (const struct sockaddr*)&server, sizeof(server)); //sends the size of the image for the client to copy
    while (remainingBytes > 0) {
        size_t sendingSize = remainingBytes > BUFFER_SIZE ? BUFFER_SIZE : remainingBytes; //ensures the packet size does not exceed the buffer size for UDP
        while (sendto(socket, (char*)currentSendPos, sendingSize, 0, (const struct sockaddr*)&server, sizeof(server)) < 0) {
            std::cout << "Error sending packet, redoing" << std::endl;
        }
        remainingBytes -= sendingSize;
        currentSendPos += sendingSize; //change pointer address to be equal to how long the current packet is
    }
    delete[] imageBuffer;
}
void recieveImage(int socket, sockaddr_in server, cv::Mat& finalImage) {
    std::cout << "Recieve function called " << std::endl;
    char sizeRecieved[sizeof(size_t)];
    size_t newBufferSize;
    int clientLength = sizeof(sockaddr_in);
    recvfrom(socket, (char*)sizeRecieved, sizeof(size_t), 0, (struct sockaddr*)&server, &clientLength); //recieves the size of the image and stores in a char array
    memcpy(&newBufferSize, sizeRecieved, sizeof(size_t));
    char* buffer = new char[newBufferSize]; //done dynamically in case image is high res and therefore requires more memory
    char* currentRecieve = &buffer[0];
    size_t remainingBytes = newBufferSize;
    while (remainingBytes > 0) {
        size_t sendingSize = remainingBytes > BUFFER_SIZE ? BUFFER_SIZE : remainingBytes;
        while (recvfrom(socket, (char*)currentRecieve, sendingSize, 0, (struct sockaddr*)&server, &clientLength) < 0) {
            std::cout << "Error sending packet, redoing" << std::endl;
        }
        remainingBytes -= sendingSize;
        currentRecieve += sendingSize; //change pointer address to be equal to how long the current packet is
        std::cout << "Amount left to recieve: " << remainingBytes << std::endl;
    }
    vector<uchar> finalImageToConvert;
    finalImageToConvert.assign(buffer, buffer + newBufferSize); //similar to
    finalImage = cv::imdecode(cv::Mat(finalImageToConvert), 1);
    std::cout << "Image recieved" << std::endl;
    delete[] buffer;
}
int main() {
    // Create a UDP socket
#ifdef _WIN32
// Initialize Winsock on Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed" << std::endl;
        return 1;
    }
#endif
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    vector<int> numbersList;
    vector<std::string> argumentsList;
    char buffer[1024];
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Prepare the server address structure
    struct sockaddr_in serverAddress, clientAddress;
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);
    vector<uchar> imageToRecieve;
    // Bind the socket to the server address
    if (::bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) //speciying that we are needing a different bind to std::bind
    {
#ifdef _WIN32
        std::cerr << "Error binding socket." << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
#endif
        return 1;
    }
    socklen_t clientLength;
    clientLength = sizeof(clientAddress);
    size_t response = recvfrom(serverSocket, buffer, 1024, MSG_WAITALL, (struct sockaddr*)&clientAddress, &clientLength);
    buffer[response] = '\0'; //size_t will be the size of the char array, so last character should be a return char
    string arguments = std::string(buffer); //to convert the arguments recieved from the console, a string is used
    std::cout << response << std::endl;
    istringstream spaceRemover(arguments); //string stream will be utilised to remove the spaces
    string toAssign;
    while (getline(spaceRemover, toAssign, ' ')) //gets each portion of the string (via stringstream) and seperates via spaces.
    {
        lowerCase(toAssign);
        argumentsList.push_back(toAssign); //toAssign is an empty string that gets overwritten, with each overwrite being a word between spaces
    }
    //the following code is required to get the size of the image, as UDP has a file size limit of 64kB when sending packets
    cv::Mat image;
    if (image.empty())
    {
        cout << "Cannot open image" << endl;
        closesocket(serverSocket);
    }
    cv::destroyWindow("Test Window");
    cv::waitKey(0);
    recieveImage(serverSocket, serverAddress, image);
    string path = argumentsList[1];
    argumentsList.erase(argumentsList.begin() + 0); //first element no longer needed for the vector
    imageProcessing* filter; //required for dynamic allocation
    Filters f;
    try
    {
        f = enumConvert(argumentsList[0]); //there is no convertion from string to enum, so a function is used instead
    }
    catch (InvalidOperationException e)
    {
        //cout << e << endl; //operator overloading should display message from e
        closesocket(serverSocket);
    }
    cv::Mat finalImage;
    try
    {
        switch (f)
        {
        case ROTATE:
            filter = new rotation(argumentsList, image); //filter type is not known at compile time, dynamic objects are used for polymorphism
            finalImage = filter->process(); //calls overrwritten process method
        case COLOURADJUST:
            filter = new ColourAdjustment(argumentsList, image);
            finalImage = filter->process();
        case BLUR:
            filter = new Blur(argumentsList, image);
            finalImage = filter->process();
        case FLIP:
            filter = new Flip(argumentsList, image);
            finalImage = filter->process();
        case RESIZE:
            filter = new Resize(argumentsList, image);
            finalImage = filter->process();
        case COLOURCONVERT:
            filter = new ColourConvert(argumentsList, image);
            finalImage = filter->process();
        default:
            cout << "Incorrect filter supplied" << endl;
            closesocket(serverSocket);
        }
    }
    
    catch (InvalidOperationException e)
    {
        //cout << "Operation of type " << e << "does not exist!" << endl;
        closesocket(serverSocket);
    }
    catch (NonNumeric e) {
        //cout << "String: " << e << " was detected when it should be numeric!" << endl;
        closesocket(serverSocket);
    }
    catch (MissingArgs e) {
        //cout << e << endl;
        closesocket(serverSocket);
    }
    
    size_t pos = path.find(".");
    sendImage(serverSocket, finalImage, serverAddress, path.substr(pos));
    return 0;
    closesocket(serverSocket);
}