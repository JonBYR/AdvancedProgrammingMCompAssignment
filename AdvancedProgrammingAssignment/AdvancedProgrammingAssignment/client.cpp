#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp> //Required for opencv functions
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link the Winsock library
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SOCKET int
#define INVALID_SOCKET -1
#endif
#define BUFFER_SIZE 1024
using namespace std;
using namespace cv;
void sendImage(int socket, Mat image, sockaddr_in server, const string& path)
{
    std::cout << "Send Function called" << std::endl;
    vector<uchar> imageToSend;
    imencode(path, image, imageToSend);
    size_t imageSize = imageToSend.size();
    char* imageBuffer = new char[imageSize]; //as size of image is not known at compile time, char array must be dynamic 
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
void recieveImage(int socket, sockaddr_in &server, Mat& finalImage) {
    char sizeRecieved[sizeof(size_t)];
    size_t newBufferSize;
    int clientLength = sizeof(sockaddr_in);
    std::cout << "Before Recieve" << std::endl;
    while (recvfrom(socket, (char*)sizeRecieved, sizeof(size_t), 0, (struct sockaddr*)&server, &clientLength) == SOCKET_ERROR) {
        std::cout << "No server has been established" << std::endl;
    } //recieves the size of the image and stores in a char array
    std::cout << "Recieve function called" << std::endl;
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
    finalImageToConvert.assign(buffer, buffer + newBufferSize); //similar to memcpy, but for vectors rather than arrays. Assigns buffers values to uchar vector
    finalImage = imdecode(Mat(finalImageToConvert), 1);
    delete[] buffer;
}
int main(int argc, char** argv) {
    if (argc > 5 || argc <= 0) {
        std::cerr << "Usage: ./data_processing_program <input_file> <filter> <required_parameters>" << std::endl;
        return 0;
    }
#ifdef _WIN32
    // Initialize Winsock on Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed" << std::endl;
        return 1;
    }
#endif

    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    //char buffer[BUFFER_SIZE];
    std::string imagePath = argv[1];
    Mat image = imread(imagePath, IMREAD_COLOR);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }
    char* messageToServer = new char[1024];
    char* messagePoint = &messageToServer[0];
    for (int i = 0; i < argc; i++) { //loops through input arguments, aside from the client file
        char* currentArg = argv[i];
        for (int j = 0; j < strlen(argv[i]); j++) {
            *messagePoint = currentArg[j]; //dereferencing pointer assigns the character of messageToServer at position messagePoint
            messagePoint++; //will cause the next character in messagetoserver to be the next index via pointer addition
        }
        *messagePoint = ' '; //dereference so that the next char after a parameter is a space
        messagePoint++;
    }
    if (image.empty())
    {
        cout << "Cannot open image" << endl;
        closesocket(clientSocket);
    }
    imshow("Test Window", image);
    waitKey(0); //opencv function needed as it allows image to stay displayed
    destroyWindow("Test Window");
    // Prepare the server address structure
    struct sockaddr_in serverAddress, clientAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    //Change this as it is deprecated
    serverAddress.sin_addr.s_addr =  inet_addr("127.0.0.1");
    socklen_t clientLength;
    clientLength = sizeof(clientAddress);
    sendto(clientSocket, (const char*)messageToServer, 1024, 0, (const struct sockaddr*)&serverAddress, sizeof(serverAddress));
    delete[] messageToServer;
    string s = argv[1];
    size_t pos = s.find(".jpg");
    sendImage(clientSocket, image, serverAddress, s.substr(pos));
    Mat finalImage;
    recieveImage(clientSocket, serverAddress, finalImage);
    imshow("FinalWindow", finalImage);
    waitKey(0);
    destroyWindow("FinalWindow");
    //imwrite is used to write image to path location
    closesocket(clientSocket);
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
