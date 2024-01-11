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
#include "FlipException.h"
#include "SizeException.h"
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
void closeOnError(const int& socket) { //call function if error occurs
    closesocket(socket);
    WSACleanup();
    exit(-1); //breaks program cleanly
}
Filters enumConvert(const string& filterName) throw(InvalidOperationException) //const used to make sure filterName does not change
{
    if (filterName == "rotation") return ROTATE; //enums are used in order to make the switch statement later in the code easier to navigate
    else if (filterName == "brightness" || filterName == "contrast" || filterName == "gamma") return COLOURADJUST;
    else if (filterName == "box" || filterName == "sharpening") return BLUR;
    else if (filterName == "flip") return FLIP;
    else if (filterName == "size") return RESIZE;
    else if (filterName == "greyscale") return COLOURCONVERT; //each enum relates to each 'family' of filters outlined in the brief, which is easier to understand for the switch
    else {
        throw InvalidOperationException(filterName); //any invalid filter names will throw this exception
    }
}
void lowerCase(string& s) { //call by reference used to adapt change the string that is passed through
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {return tolower(c); }); //standard transform function in c++ to convert to lower case
}
void sendImage(int socket, cv::Mat image, sockaddr_in server, const string& path) //call by reference is more memory efficent and const used so there is no danger of string being overwritten
{
    vector<uchar> imageToSend;
    cv::imencode(path, image, imageToSend); //cv function converts mat to uchar
    size_t imageSize = imageToSend.size();
    uchar* currentSendPos = &imageToSend[0]; //starting position to send image data 
    size_t remainingBytes = imageSize; //size of image left to send (that is partitioned in packets)
    char sizeBuffer[sizeof(size_t)];
    memcpy(sizeBuffer, &imageSize, sizeof(size_t)); //stores the size of the image as a char
    sendto(socket, (const char*)sizeBuffer, sizeof(size_t), 0, (const struct sockaddr*)&server, sizeof(server)); //sends the size of the image for the client to copy, this is because the client needs to know the size of the recieved image as it will not at compile time
    while (remainingBytes > 0) { //while there are still packets to send to the client
        size_t sendingSize = remainingBytes > BUFFER_SIZE ? BUFFER_SIZE : remainingBytes; //ensures the packet size does not exceed the buffer size for UDP
        while (sendto(socket, (char*)currentSendPos, sendingSize, 0, (const struct sockaddr*)&server, sizeof(server)) < 0) {
            std::cout << "Error sending packet, retrying" << std::endl; //if sendto fails it returns -1
            break;
        }
        remainingBytes -= sendingSize;
        currentSendPos += sendingSize; //change pointer address to be equal to how long the current packet is, which therefore changes the position of the char array
        this_thread::sleep_for(chrono::milliseconds(1)); //asks main thread to wait 1 millisecond to ensure that server has correctly recieved the packet
    }
}
void recieveImage(int socket, sockaddr_in server, cv::Mat& processImage) {
    char sizeRecieved[sizeof(size_t)];
    size_t newBufferSize;
    int clientLength = sizeof(sockaddr_in);
    recvfrom(socket, (char*)sizeRecieved, sizeof(size_t), 0, (struct sockaddr*)&server, &clientLength); //recieves the size of the image and stores in a char array
    memcpy(&newBufferSize, sizeRecieved, sizeof(size_t)); //specifies in newBufferSize the size of the image, recieved from client
    char* buffer = new char[newBufferSize]; //done dynamically as server does not know size of image recieved at compile time and only now knows from the client
    char* currentRecieve = &buffer[0];
    size_t remainingBytes = newBufferSize;
    while (remainingBytes > 0) {
        size_t sendingSize = remainingBytes > BUFFER_SIZE ? BUFFER_SIZE : remainingBytes;
        while (recvfrom(socket, (char*)currentRecieve, sendingSize, 0, (struct sockaddr*)&server, &clientLength) < 0) { //if recieve from fails it returns -1
            std::cout << "Error sending packet, redoing" << std::endl;
        }
        remainingBytes -= sendingSize;
        currentRecieve += sendingSize; //change pointer address to be equal to how long the current packet is meaning next packet will start in the correct sequence in the char array
        std::cout << "Amount left to recieve: " << remainingBytes << std::endl;
    }
    vector<uchar> processImageToConvert;
    processImageToConvert.assign(buffer, buffer + newBufferSize); //assigns the char buffer containing the image to a uchar vector, specifying the size of the vector as the size of buffer
    processImage = cv::imdecode(cv::Mat(processImageToConvert), 1); //opencv function that converts from uchar to Mat
    delete[] buffer;
}
int main() {
    // Create a UDP socket
#ifdef _WIN32
// Initialize Winsock on Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { //initilise program
        std::cerr << "Winsock initialization failed" << std::endl;
        return 1;
    }
#endif
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0); //establish socket
    vector<int> numbersList;
    vector<std::string> argumentsList;
    char buffer[1024]; //buffer will store input arguments from client
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

    // Prepare the server address structure
    struct sockaddr_in serverAddress, clientAddress;
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; //takes any ip address
    serverAddress.sin_port = htons(12345);
    vector<uchar> imageToRecieve;
    // Bind the socket to the server address
    if (::bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) //speciying that we are needing a different bind to std::bind, required due to using namespace std 
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
    std::cout << "Waiting for response" << std::endl;
    if (recvfrom(serverSocket, buffer, 1024, 0, (struct sockaddr*)&clientAddress, &clientLength) == SOCKET_ERROR) cout << WSAGetLastError() << std::endl;
    string arguments = std::string(buffer); //to convert the arguments recieved from the console, a string is used
    istringstream spaceRemover(arguments); //string stream will be utilised to remove the spaces
    string toAssign;
    while (getline(spaceRemover, toAssign, ' ')) //gets each portion of the string (via stringstream) and seperates via spaces.
    {
        lowerCase(toAssign); //to make string checks easier, all strings are converted to lower case
        argumentsList.push_back(toAssign); //toAssign is an empty string that gets overwritten. Each time toAssign is called, it will take in a the characters of a string until it reaches a space, then the next iteration is performed
    }
    cv::Mat image;
    recieveImage(serverSocket, serverAddress, image); //call by referenced used for image, so when the argument value changes, image value also changes
    string path = argumentsList[2];
    argumentsList.erase(argumentsList.begin() + 2);
    argumentsList.erase(argumentsList.begin() + 1); //third, second and first element no longer needed for the vector as it contains server ip, image path and client executable respectively
    argumentsList.erase(argumentsList.begin() + 0);
    imageProcessing* filter; //required for dynamic allocation
    Filters f;
    try
    { //argumentsList[0] should now be the filter used
        f = enumConvert(argumentsList[0]); //there is no convertion from string to enum, so a function is used instead
    }
    catch (InvalidOperationException e)
    {
        cout << e << endl; //operator overloading should display message from e when performing cout
        closeOnError(serverSocket);
    }
    cv::Mat finalImage;
    try
    {
        switch (f)
        {
        case ROTATE:
            filter = new rotation(argumentsList, image); //filter type is not known at compile time, dynamic objects are used for polymorphism
            finalImage = filter->process(); //calls overrwritten process method which is unique to each child, perfoming it's own functionality
            delete filter; //dynamic pointers must be deleted after use
            break;
        case COLOURADJUST:
            filter = new ColourAdjustment(argumentsList, image);
            finalImage = filter->process();
            delete filter;
            break;
        case BLUR:
            filter = new Blur(argumentsList, image);
            finalImage = filter->process();
            delete filter;
            break;
        case FLIP:
            filter = new Flip(argumentsList, image);
            finalImage = filter->process();
            delete filter;
            break;
        case RESIZE:
            filter = new Resize(argumentsList, image);
            finalImage = filter->process();
            delete filter;
            break;
        case COLOURCONVERT:
            filter = new ColourConvert(argumentsList, image);
            finalImage = filter->process();
            delete filter;
            break;
        default:
            cout << "Incorrect filter supplied" << endl; //should any of the enums not be called, an invalid argument was passed which is then caught here
            closeOnError(serverSocket);
        }
    }
    catch (InvalidOperationException e) //any exceptions thrown in the objects are caught here and use their overloaded << operator
    {
        cout << e << endl;
        closeOnError(serverSocket);
    }
    catch (NonNumeric e) {
        cout << e << endl;
        closeOnError(serverSocket);
    }
    catch (MissingArgs e) {
        cout << e << endl;
        closeOnError(serverSocket);
    }
    catch (FlipException e) {
        cout << e << endl;
        closeOnError(serverSocket);
    }
    catch (SizeException e) {
        cout << e << endl;
        closeOnError(serverSocket);
    }
    size_t pos = path.find(".jpg"); //image extension required for the encode operation
    sendImage(serverSocket, move(finalImage), clientAddress, path.substr(pos)); //move used as finalImage is not being used again after function called, so if passed to parameter
    return 0;
    closesocket(serverSocket);
}