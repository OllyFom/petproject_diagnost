#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <time.h>
int PACKET_SIZE = 64;
#define TIMEOUT 1;
unsigned short mychecksum(unsigned short * buffer,int size){ // сумма прочитанных бит информации
    unsigned long sum = 0;    
    while (size > 1){
        sum += *buffer++;
        size -= 2;
    }
    if (size){
        sum += *(unsigned char *) buffer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}
int main (int argc, char** argv){
    if (argc != 2){
        printf("Can`t check empty adress\n");
        return 1;}
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0){
        perror("can`t create socket");
        return 1;
    }
    struct sockaddr_in dest_addr; // where to
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &dest_addr.sin_addr); // type - what convert - where 
    char packet[PACKET_SIZE];
    memset(packet, 0, PACKET_SIZE);
    struct icmphdr *icmp_hdr = (struct icmphdr*)packet; /// internet messeges
    icmp_hdr->type = ICMP_ECHO; // echo requests
    icmp_hdr->code = 0; // packet code ???
    icmp_hdr->un.echo.id = getpid();
    icmp_hdr->checksum = mychecksum((unsigned short *) icmp_hdr, PACKET_SIZE);
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    if (sendto(sockfd, packet, PACKET_SIZE, 0 /* flags???*/, (struct sockaddr*)&dest_addr, sizeof (dest_addr)) < 0){
        perror("sendto");
        return 1;
    }
    char recv_buffer[1500];
    struct sockaddr_in recv_addr; // where from, need to return
    socklen_t addr_len = sizeof(recv_addr);
    fd_set read_fds; // descriptor
    FD_ZERO(&read_fds); // clear descriptor
    FD_SET(sockfd, &read_fds); // set 
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    int select_result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout); // monitoring ready status of descriptors (&read_fds, NULL, NULL) - are descriptors
    if (select_result < 0){
        perror("select");
        return 1;
    }
    else if(select_result == 0){
        printf("Request timed out.\n");
        return 1;
    }
    if (recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&recv_addr, &addr_len) < 0){ //receive data from our socket and check
        perror("recvfrom");
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double rtt = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
    printf("Reply from %s: bytes=%d time=%.2f ms\n", inet_ntoa(recv_addr.sin_addr)/*reverce inet_pton */, PACKET_SIZE, rtt);

    close(sockfd);
    return 0;
}