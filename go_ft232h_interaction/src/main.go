package main

import (
	"log"
	"unsafe"
)

/*

#cgo LDFLAGS: -L../../ft232h_interaction -llibmpsse
#include "../../ft232h_interaction/ftd2xx.h"
#include "../../ft232h_interaction/libmpsse_spi.h"
#include <windows.h>

*/
import "C"

var spi_channel_config unsafe.Pointer
var spi_channel_handle C.FT_HANDLE

func main() {
	log.Println("*")
	initialize()

	var num_of_spi_channels C.DWORD
	ft_status := C.SPI_GetNumChannels(&num_of_spi_channels)
	if ft_status != C.FT_OK {
		log.Println("Unable to get number of spi channels for device:", ft_status)
		deinitialize()
		return
	}
	log.Println("Number of spi channels for device:", num_of_spi_channels)

	ft_status = C.SPI_OpenChannel(0, &spi_channel_handle)
	if ft_status != C.FT_OK {
		log.Println("Unable to open device spi channel:", ft_status)
		deinitialize()
		return
	}
	log.Println("Device spi channel opened")

	spi_channel_config_casted := (*C.ChannelConfig)(spi_channel_config) // Cast to the C struct type
	spi_channel_config_casted.ClockRate = 2000000
	spi_channel_config_casted.LatencyTimer = 1
	spi_channel_config_casted.configOptions = C.SPI_CONFIG_OPTION_MODE0 | C.SPI_CONFIG_OPTION_CS_DBUS3 | C.SPI_CONFIG_OPTION_CS_ACTIVELOW
	spi_channel_config_casted.Pin = 0b00000000000010110000000000001011
	spi_channel_config_casted.currentPinState = 0b0000000000001011
	ft_status = C.SPI_InitChannel(spi_channel_handle, spi_channel_config_casted)
	if ft_status != C.FT_OK {
		log.Println("Unable to initialize device spi channel:", ft_status)
		deinitialize()
		return
	}
	log.Println("Device spi channel initialized")

	var device_busy C.BOOL
	ft_status = C.SPI_IsBusy(spi_channel_handle, &device_busy)
	if ft_status != C.FT_OK {
		log.Println("Unable to determine if device spi is busy:", ft_status)
		deinitialize()
		return
	}
	log.Println("Device SPI busy:", device_busy)
	if device_busy == 1 {
		deinitialize()
		return
	}

	var gpio_line_direction C.UCHAR = 0b00001011
	var gpio_line_value C.UCHAR = 0b00000000
	ft_status = C.FT_WriteGPIO(spi_channel_handle, gpio_line_direction, gpio_line_value)
	if ft_status != C.FT_OK {
		log.Println("Unable write gpio:", ft_status)
		deinitialize()
		return
	}
	log.Println("Wrote GPIO")

	data := "Hola Mundo!"
	data_size := len(data)
	buffer := (*C.UCHAR)(C.malloc(C.size_t(data_size)))
	defer C.free(unsafe.Pointer(buffer)) // Free the buffer when done
	// Set values in the buffer
	for i := 0; i < data_size; i++ {
		ptr := (*C.UCHAR)(unsafe.Pointer(uintptr(unsafe.Pointer(buffer)) + uintptr(i)))
		*ptr = C.UCHAR(data[i])
	}

	var bytes_transferred C.DWORD = 0
	var size_to_transfer C.DWORD = C.DWORD(data_size)

	ft_status = C.SPI_Write(spi_channel_handle, buffer, size_to_transfer, &bytes_transferred, C.SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|C.SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE|C.SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE)
	if ft_status != C.FT_OK {
		log.Println("Unable write to device spi:", ft_status)
		deinitialize()
		return
	}
	log.Println("Bytes written to device spi:", bytes_transferred)

	deinitialize()
}

func initialize() {
	log.Println("Initializing")
	C.Init_libMPSSE()
	spi_channel_config = C.malloc(C.sizeof_ChannelConfig)
}

func deinitialize() {
	log.Println("Deinitializing")

	C.Cleanup_libMPSSE()
	if spi_channel_handle != nil {
		C.SPI_CloseChannel(spi_channel_handle)
	}
	C.free(spi_channel_config)
}
