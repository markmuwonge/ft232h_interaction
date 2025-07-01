#include <windows.h>

#include <stdio.h>

#include "ftd2xx.h"

#include <stdlib.h>

#include "libmpsse_spi.h"

void init();
void deinit();

static FT_STATUS ft_status = 0;
static FT_HANDLE spi_channel_handle = NULL;
static ChannelConfig* spi_channel_config;
int main() {
    init();

    DWORD num_of_spi_channels;
    ft_status = SPI_GetNumChannels(&num_of_spi_channels);
    if (ft_status != FT_OK){
        printf("Unable to get number of spi channels for device: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Number of spi channels for device: %d\n", num_of_spi_channels);
    if (num_of_spi_channels <=0){
        printf("Unable to get spi channels for device: %d\n", ft_status);
        deinit();
        return;
    }
    
    ft_status = SPI_OpenChannel(0, &spi_channel_handle);
    if (ft_status != FT_OK){
        printf("Unable to open device spi channel: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Device spi channel opened\n");

    spi_channel_config->ClockRate = 2000000;
    spi_channel_config->LatencyTimer = 1;
    spi_channel_config->configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    spi_channel_config->Pin = 0b00000000000010110000000000001011;
    spi_channel_config->currentPinState = 0b0000000000001011;

    ft_status = SPI_InitChannel(spi_channel_handle, spi_channel_config);
    if (ft_status != FT_OK){
        printf("Unable to initialize device spi channel: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Device spi channel initialized\n");

    BOOL device_busy;
    ft_status = SPI_IsBusy(spi_channel_handle, &device_busy);
    if (ft_status != FT_OK){
        printf("Unable to determine if device spi is busy: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Device SPI busy: %d\n", device_busy);
    if (device_busy){
        deinit();
        return;
    }

    UCHAR buffer[] = "Hello World!";
    DWORD bytes_transferred = 0;
    ft_status = SPI_Write(spi_channel_handle, buffer, sizeof(buffer)/sizeof(buffer[0]) , &bytes_transferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
    if (ft_status != FT_OK){
        printf("Unable write to device spi: %d\n", ft_status);
        deinit();
        return;
    }
    printf("Bytes written to device spi: %d\n", bytes_transferred);

  deinit();
  return 0;
}

void init(){
    Init_libMPSSE();
    printf("Init\n");
    spi_channel_config = malloc(sizeof(ChannelConfig));
}

void deinit(){
    Cleanup_libMPSSE();
    if (spi_channel_handle != NULL){
        SPI_CloseChannel(spi_channel_handle);
    }
    free(spi_channel_config);
    printf("Deinit\n");
}