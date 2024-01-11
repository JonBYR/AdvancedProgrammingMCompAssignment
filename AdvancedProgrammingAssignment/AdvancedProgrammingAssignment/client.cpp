#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp> //Required for opencv functions
#include <thread>
#include <chrono>
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
void closeOnError(const int& socket) { //call function if error occurs
    closesocket(socket);
    WSACleanup();
    exit(-1); //breaks program cleanly
}
void sendImage(int socket, Mat image, sockaddr_in server, const string& path) //path should not be modified
{
    vector<uchar> imageToSend;
    imencode(path, image, imageToSend);
    size_t imageSize = imageToSend.size();
    uchar* currentSendPos = &imageToSend[0]; //starting position to send image data 
    size_t remainingBytes = imageSize; //size of image left to send (that is partitioned in packets)
    char sizeBuffer[sizeof(size_t)];
    memcpy(sizeBuffer, &imageSize, sizeof(size_t)); //stores the size of the image as a char
    sendto(socket, (const char*)sizeBuffer, sizeof(size_t), 0, (const struct sockaddr*)&server, sizeof(server)); //sends the size of the image for the server to copy
    while (remainingBytes > 0) {
        size_t sendingSize = remainingBytes > BUFFER_SIZE ? BUFFER_SIZE : remainingBytes; //ensures the packet size does not exceed the buffer size for UDP
        while (sendto(socket, (char*)currentSendPos, sendingSize, 0, (const struct sockaddr*)&server, sizeof(server)) < 0) {
            std::cout << "Error sending packet, redoing" << std::endl;
        }
        remainingBytes -= sendingSize;
        currentSendPos += sendingSize; //change pointer address to be equal to how long the current packet is, which therefore changes the position of the char array
        this_thread::sleep_for(chrono::milliseconds(1)); //asks main thread to wait 1 millisecond to ensure that server has correctly recieved the packet
    }
}
void recieveImage(int socket, sockaddr_in &server, Mat& finalImage) {
    bool serverSend = false;
    char sizeRecieved[sizeof(size_t)];
    size_t newBufferSize;
    int clientLength = sizeof(sockaddr_in);
    while (!serverSend) //client is waiting for the server to send information
    {
        if (recvfrom(socket, (char*)sizeRecieved, sizeof(size_t), 0, (struct sockaddr*)&server, &clientLength) == SOCKET_ERROR) { //if server has not yet send information
            std::cout << "No server has been established. Have you run the server? Is the port given the same as the server?" << std::endl;
            closeOnError(socket);
        }
        else {
            serverSend = true;
            break; //stop while loop as server is now sending information
            //as the if statement has recieved information from the server at this point, the value of sizeRecieved has changed
        }
    }
     //recieves the size of the image and stores in a char array
    memcpy(&newBufferSize, sizeRecieved, sizeof(size_t)); //sizeRecieved contains the size of the image that is passed by reference to newBufferSize
    char* buffer = new char[newBufferSize]; //done dynamically as image size is not known until sent from server and copied to newBufferSize
    char* currentRecieve = &buffer[0];
    size_t remainingBytes = newBufferSize;
    while (remainingBytes > 0) {
        size_t sendingSize = remainingBytes > BUFFER_SIZE ? BUFFER_SIZE : remainingBytes;
        while (recvfrom(socket, (char*)currentRecieve, sendingSize, 0, (struct sockaddr*)&server, &clientLength) < 0) {
            std::cout << "Error sending packet, redoing" << std::endl;
        }
        remainingBytes -= sendingSize;
        currentRecieve += sendingSize; //change pointer address to be equal to how long the current packet is meaning next packet will start in the correct sequence in the char array
        std::cout << "Amount left to recieve: " << remainingBytes << std::endl;
    }
    vector<uchar> finalImageToConvert;
    finalImageToConvert.assign(buffer, buffer + newBufferSize); 
    finalImage = imdecode(Mat(finalImageToConvert), 1);
    delete[] buffer; //dynamic array must be deleted after use to prevent memory leak
}
int main(int argc, char** argv) {
    if (argc > 6 || argc <= 3) { //if the command line arguments are less than 4 (4th argument is filter) and greater than 6 (max number of arguments) show the statement below on command line
        std::cerr << "Usage for program: ./data_processing_program <IP Address:Port> <input_file> <filter> <required_parameters>" << std::endl;
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
    std::string imagePath = argv[2]; //stores path for later (3rd argument
    Mat image = imread(imagePath, IMREAD_COLOR); //loads image in colour
    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }
    char* messageToServer = new char[1024];
    char* messagePoint = &messageToServer[0];
    for (int i = 0; i < argc; i++) { //loops through all arguments
        char* currentArg = argv[i];
        for (int j = 0; j < strlen(argv[i]); j++) {
            *messagePoint = currentArg[j]; //dereferencing pointer assigns the character of messageToServer at position messagePoint
            messagePoint++; //will cause the next character in messagetoserver to be the next index via pointer addition
        }
        *messagePoint = ' '; //dereference so that the next char after a parameter is a space
        messagePoint++;
    }
    *messagePoint = '\0'; //assigning a null character at the end of the c string to signify we are out of arguments
    if (image.empty())
    {
        cout << "Cannot open image" << endl;
        closeOnError(clientSocket);
    }
    imshow("Test Window", image);
    waitKey(0); //opencv function needed as it allows image to stay displayed
    destroyWindow("Test Window");
    // Prepare the server address structure
    struct sockaddr_in serverAddress, clientAddress;
    serverAddress.sin_family = AF_INET;
    string ipAndPort = argv[1]; //ip address and port are contained in the second argument from the user
    if (ipAndPort.find(':') == std::string::npos) //if a colon does not exist the user has incorrectly submitted the server ip
    {
        cout << "Ip and Port have been provided incorrectly. Please use ip:port" << std::endl;
        closeOnError(clientSocket);
    }
    string port = ipAndPort.substr(ipAndPort.find(':') + 1); //contains a substring containing all numbers after :
    int portNumber;
    try {
        portNumber = stoi(port); //check port is a number
    }
    catch (...) { //if there is an error performing the stoi operation, the catch statement will catch this error to break the code cleanly
        cout << "Port should only contain integer values" << std::endl; //this clauses catches all exceptions thrown from try, however as one operation is used, only one error can be caught, therefore this catch should be valid
        closeOnError(clientSocket);
    }
    if (portNumber < 0) { //port is an unsigned short meaning it cannot be negative
        cout << "Port should not be less than 0" << std::endl;
        closeOnError(clientSocket);
    }
    string ip = ipAndPort.substr(0, ipAndPort.find(":")); //ip will start from the start of the string until the :
    serverAddress.sin_port = htons((u_short)portNumber);
    serverAddress.sin_addr.s_addr =  inet_addr(ip.c_str()); //ip must be a c string
    socklen_t clientLength;
    clientLength = sizeof(clientAddress);
    if (sendto(clientSocket, (const char*)messageToServer, 1024, 0, (const struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) //sends input arguments for server to process as the if statement is passed
    {
        std::cout << "Error connecting to server. Have you checked that your IP address and port are correct?" << std::endl; //if the if statement is called then either the IP address entered is incorrect or the port
        closeOnError(clientSocket);
    }
    delete[] messageToServer;
    size_t pos = imagePath.find(".jpg"); //pos will be used to get the substring that denotes the image extension
    sendImage(clientSocket, move(image), serverAddress, imagePath.substr(pos)); //image is no longer used after this function so move operation used for better memory efficency
    Mat finalImage;
    recieveImage(clientSocket, serverAddress, finalImage); //finalImage will be adapted by call by reference
    imshow("FinalWindow", finalImage); //shows image in new window
    waitKey(0);
    destroyWindow("FinalWindow");
    imwrite(".\\ProcessedImage.jpg", finalImage); //stores image in new file
    closesocket(clientSocket);
}