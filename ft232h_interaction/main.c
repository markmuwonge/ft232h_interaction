#include <windows.h>
#include <stdio.h>
#include "ftd2xx.h"
#include <stdlib.h>
#include <stdint.h>

int main(){
	
    FT_HANDLE ft_handle;
    if (FT_Open(0, &ft_handle) != FT_OK){
    	printf("Unable to open device\n");
    	return;
    }
    printf("Device opened\n");

    if (FT_ResetDevice(ft_handle) != FT_OK){
  		printf("Unable to reset device\n");
    	goto end;
    }
    printf("Device reset\n");

    if (FT_SetBaudRate (ft_handle, FT_BAUD_9600) != FT_OK){
    	printf("Unable to set baud rate\n");
    	goto end;
    }
    printf("Device baud rate set\n");

    if (FT_SetBitMode(ft_handle, 0,  FT_BITMODE_RESET) != FT_OK){
    	printf("Unable to reset device MPSSE\n");
    	goto end;
    }
    printf("Device MPSSE reset\n");

    if (FT_SetBitMode(ft_handle, 0b11111111, FT_BITMODE_ASYNC_BITBANG) != FT_OK){
    	printf("Unable to set device bit mode\n");
    	goto end;
    }
    printf("Device bit mode set\n");



    uint8_t data[] = {5};
    DWORD bytes_written;

    if (FT_Write(ft_handle, data,   sizeof(data) / sizeof(data[0]),  &bytes_written) != FT_OK){
    		printf("Unable to write bytes");
    }
    printf("Bytes writen: %d\n", bytes_written);

 

end:

    FT_Close(ft_handle);
	return 0;
}