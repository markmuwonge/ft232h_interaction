#include <windows.h>

#include <stdio.h>

#include "ftd2xx.h"

#include <stdlib.h>

#include "libmpsse_i2c.h"

#include <windows.h>

// #define SLAVE_ADDRESS 0x50
#define SLAVE_ADDRESS 0x7f

void init();
void deinit();

static FT_STATUS ft_status = 0;
static FT_HANDLE i2c_channel_handle = NULL;
static ChannelConfig* i2c_channel_config;


int main() {
    init();

    DWORD num_of_i2c_channels;
    ft_status = I2C_GetNumChannels(&num_of_i2c_channels);
    if (ft_status != FT_OK){
        printf("Unable to get number of i2c channels for device: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Number of i2c channels for device: %d\n", num_of_i2c_channels);
    if (num_of_i2c_channels <=0){
        printf("Unable to get i2c channels for device: %d\n", ft_status);
        deinit();
        return;
    }
    
    ft_status = I2C_OpenChannel(0, &i2c_channel_handle);
    if (ft_status != FT_OK){
        printf("Unable to open device i2c channel: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Device i2c channel opened\n");

    i2c_channel_config->ClockRate = I2C_CLOCK_STANDARD_MODE;
    i2c_channel_config->LatencyTimer = 50;
    i2c_channel_config->Options = I2C_TRANSFER_OPTIONS_NO_ADDRESS;  
    i2c_channel_config->Pin = 0b00000000000000110000000000000011; //D0 SCL, D1, SDA
    i2c_channel_config->currentPinState = 0b0000000000000011;

    ft_status = I2C_InitChannel(i2c_channel_handle, i2c_channel_config);
    if (ft_status != FT_OK){
        printf("Unable to initialize device i2c channel: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Device i2c channel initialized\n");

    UCHAR buffer[] = "Hello World!";
    DWORD bytes_transferred = 0;
    UCHAR recv_buffer[255];
    DWORD bytes_received = 0;

    while(1){
        ft_status = I2C_DeviceWrite(
            i2c_channel_handle,
            SLAVE_ADDRESS,
            sizeof(buffer)/sizeof(buffer[0]),
            buffer,
            &bytes_transferred,
            I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_STOP_BIT
        );
        if (ft_status != FT_OK){
            // printf("Unable write to device i2c: %d\n", ft_status);
            // deinit();
            // return;
        }
        printf("Bytes written to device i2c: %d\n", bytes_transferred);


        // I2C_DeviceRead(
        //     i2c_channel_handle,
        //     0x7F,
        //     255,
        //     recv_buffer,
        //     &bytes_received,
        //     I2C_TRANSFER_OPTIONS_NO_ADDRESS
        // )


        Sleep(1000);
    }



  deinit();
  return 0;
}

void init(){
    Init_libMPSSE();
    printf("Init\n");
    i2c_channel_config = malloc(sizeof(ChannelConfig));
}

void deinit(){
    Cleanup_libMPSSE();
    if (i2c_channel_handle != NULL){
        I2C_CloseChannel(i2c_channel_handle);
    }
    free(i2c_channel_config);
    printf("Deinit\n");
}