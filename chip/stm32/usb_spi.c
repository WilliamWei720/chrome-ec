/* Copyright 2014 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "common.h"
#include "gpio.h"
#include "link_defs.h"
#include "registers.h"
#include "spi.h"
#include "usb_descriptor.h"
#include "usb_hw.h"
#include "usb_spi.h"
#include "util.h"

/* Forward declare platform specific functions. */
static bool usb_spi_received_packet(void);
static bool usb_spi_transmitted_packet(void);
static void usb_spi_read_packet(struct usb_spi_packet_ctx *packet);
static void usb_spi_write_packet(struct usb_spi_packet_ctx *packet);

/*
 * Map EC error codes to USB_SPI error codes.
 *
 * @param error     EC error code
 *
 * @returns         USB SPI error code based on the mapping.
 */
static int16_t usb_spi_map_error(int error)
{
	switch (error) {
	case EC_SUCCESS:
		return USB_SPI_SUCCESS;
	case EC_ERROR_TIMEOUT:
		return USB_SPI_TIMEOUT;
	case EC_ERROR_BUSY:
		return USB_SPI_BUSY;
	default:
		return USB_SPI_UNKNOWN_ERROR | (error & 0x7fff);
	}
}

/*
 * Read data into the receive buffer.
 *
 * @param dst       Destination receive context we are writing data to.
 * @param src       Source packet context we are reading data from.
 *
 * @returns         USB_SPI_RX_DATA_OVERFLOW if the source packet is too large
 */
static int usb_spi_read_usb_packet(struct usb_spi_transfer_ctx *dst,
				   const struct usb_spi_packet_ctx *src)
{
	size_t max_read_length = dst->transfer_size - dst->transfer_index;
	size_t bytes_in_buffer = src->packet_size - src->header_size;
	const uint8_t *packet_buffer = src->bytes + src->header_size;

	if (bytes_in_buffer > max_read_length) {
		/*
		 * An error occurred, we should not receive more data than
		 * the buffer can support.
		 */
		return USB_SPI_RX_DATA_OVERFLOW;
	}
	memcpy(dst->buffer + dst->transfer_index, packet_buffer,
	       bytes_in_buffer);

	dst->transfer_index += bytes_in_buffer;
	return USB_SPI_SUCCESS;
}

/*
 * Fill the USB packet with data from the transmit buffer.
 *
 * @param dst       Destination packet context we are writing data to.
 * @param src       Source transmit context we are reading data from.
 */
static void usb_spi_fill_usb_packet(struct usb_spi_packet_ctx *dst,
				    struct usb_spi_transfer_ctx *src)
{
	size_t transfer_size = src->transfer_size - src->transfer_index;
	size_t max_buffer_size = USB_MAX_PACKET_SIZE - dst->header_size;
	uint8_t *packet_buffer = dst->bytes + dst->header_size;

	if (transfer_size > max_buffer_size)
		transfer_size = max_buffer_size;

	memcpy(packet_buffer, src->buffer + src->transfer_index, transfer_size);

	dst->packet_size = dst->header_size + transfer_size;
	src->transfer_index += transfer_size;
}

/*
 * Setup the USB SPI state to start a new SPI transfer.
 *
 * @param config        USB SPI config
 * @param write_count   Number of bytes to write in the SPI transfer
 * @param read_count    Number of bytes to read in the SPI transfer
 */
static void usb_spi_setup_transfer(size_t write_count, size_t read_count)
{
	/* Reset any status code. */
	usb_spi_state.status_code = USB_SPI_SUCCESS;

	/* Reset the write and read counts. */
	usb_spi_state.spi_write_ctx.transfer_size = write_count;
	usb_spi_state.spi_write_ctx.transfer_index = 0;
	usb_spi_state.spi_read_ctx.transfer_size = read_count;
	usb_spi_state.spi_read_ctx.transfer_index = 0;
}

/*
 * Handle USB events that will reset the USB SPI state.
 *
 * @param config        USB SPI config
 */
static void usb_spi_reset_interface(void)
{
	/* Setup a 0 byte transfer to clear the contexts. */
	usb_spi_setup_transfer(0, 0);
}

/*
 * Returns if the response transfer is in progress.
 *
 * @param config        USB SPI config
 *
 * @returns         	True if a response transfer is in progress.
 */
static bool usb_spi_response_in_progress(void)
{
	if ((usb_spi_state.mode == USB_SPI_MODE_START_RESPONSE) ||
	    (usb_spi_state.mode == USB_SPI_MODE_CONTINUE_RESPONSE)) {
		return true;
	}
	return false;
}

/*
 * Prep the state to construct a new response. This sets the transfer
 * contexts, the mode, and status code. If a non-zero status code is
 * returned, then no payload will be transmitted.
 *
 * @param config        USB SPI config
 * @param status_code	status code to set for the response.
 */
static void setup_transfer_response(uint16_t status_code)
{
	usb_spi_state.status_code = status_code;
	usb_spi_state.spi_read_ctx.transfer_index = 0;
	usb_spi_state.mode = USB_SPI_MODE_START_RESPONSE;

	/* If an error occurred, transmit an empty start packet. */
	if (status_code != USB_SPI_SUCCESS)
		usb_spi_state.spi_read_ctx.transfer_size = 0;
}

/*
 * Constructs the response packet containing the SPI configuration.
 *
 * @param config        USB SPI config
 * @param packet        Packet buffer we will be transmitting.
 */
static void create_spi_config_response(struct usb_spi_packet_ctx *packet)
{
	/* Construct the response packet. */
	packet->rsp_config.packet_id = USB_SPI_PKT_ID_RSP_USB_SPI_CONFIG;
	packet->rsp_config.max_write_count = USB_SPI_MAX_WRITE_COUNT;
	packet->rsp_config.max_read_count = USB_SPI_MAX_READ_COUNT;
	/* Set the feature flags. */
	packet->rsp_config.feature_bitmap = 0;
#ifndef CONFIG_SPI_HALFDUPLEX
	packet->rsp_config.feature_bitmap |=
		USB_SPI_FEATURE_FULL_DUPLEX_SUPPORTED;
#endif
	packet->packet_size = sizeof(struct usb_spi_response_configuration_v2);
}

static void create_spi_chip_select_response(struct usb_spi_packet_ctx *packet)
{
	/* Construct the response packet. */
	packet->rsp_cs.packet_id = USB_SPI_PKT_ID_RSP_CHIP_SELECT;
	packet->rsp_cs.status_code = 0;
	packet->packet_size = sizeof(packet->rsp_cs);
}

/*
 * If we have a transfer response in progress, this will construct the
 * next entry. If no transfer is in progress or if we are unable to
 * create the next packet, it will not modify tx_packet.
 *
 * @param config        USB SPI config
 * @param packet        Packet buffer we will be transmitting.
 */
static void
usb_spi_create_spi_transfer_response(struct usb_spi_packet_ctx *transmit_packet)
{
	if (!usb_spi_response_in_progress())
		return;

	if (usb_spi_state.spi_read_ctx.transfer_index == 0) {
		/* Transmit the first packet with the status code. */
		transmit_packet->header_size =
			offsetof(struct usb_spi_response_v2, data);
		transmit_packet->rsp_start.packet_id =
			USB_SPI_PKT_ID_RSP_TRANSFER_START;
		transmit_packet->rsp_start.status_code =
			usb_spi_state.status_code;

		usb_spi_fill_usb_packet(transmit_packet,
					&usb_spi_state.spi_read_ctx);
	} else if (usb_spi_state.spi_read_ctx.transfer_index <
		   usb_spi_state.spi_read_ctx.transfer_size) {
		/* Transmit the continue packets. */
		transmit_packet->header_size =
			offsetof(struct usb_spi_continue_v2, data);
		transmit_packet->rsp_continue.packet_id =
			USB_SPI_PKT_ID_RSP_TRANSFER_CONTINUE;
		transmit_packet->rsp_continue.data_index =
			usb_spi_state.spi_read_ctx.transfer_index;

		usb_spi_fill_usb_packet(transmit_packet,
					&usb_spi_state.spi_read_ctx);
	}
	if (usb_spi_state.spi_read_ctx.transfer_index <
	    usb_spi_state.spi_read_ctx.transfer_size) {
		usb_spi_state.mode = USB_SPI_MODE_CONTINUE_RESPONSE;
	} else {
		usb_spi_state.mode = USB_SPI_MODE_IDLE;
	}
}

/*
 * Process the rx packet.
 *
 * @param config        USB SPI config
 * @param packet        Received packet to process.
 */
static void usb_spi_process_rx_packet(struct usb_spi_packet_ctx *packet)
{
	if (packet->packet_size < USB_SPI_MIN_PACKET_SIZE) {
		/* No valid packet exists smaller than the packet id. */
		setup_transfer_response(USB_SPI_RX_UNEXPECTED_PACKET);
		return;
	}
	/* Reset the mode until we've processed the packet. */
	usb_spi_state.mode = USB_SPI_MODE_IDLE;

	switch (packet->packet_id) {
	case USB_SPI_PKT_ID_CMD_GET_USB_SPI_CONFIG: {
		/* The host requires the SPI configuration. */
		usb_spi_state.mode = USB_SPI_MODE_SEND_CONFIGURATION;
		break;
	}
	case USB_SPI_PKT_ID_CMD_RESTART_RESPONSE: {
		/*
		 * The host has requested the device restart the last response.
		 * This is used to recover from lost USB packets without
		 * duplicating SPI transfers.
		 */
		setup_transfer_response(usb_spi_state.status_code);
		break;
	}
	case USB_SPI_PKT_ID_CMD_TRANSFER_START: {
		/* The host started a new USB SPI transfer */
		size_t write_count = packet->cmd_start.write_count;
		size_t read_count = packet->cmd_start.read_count;

		if (!usb_spi_state.enabled) {
			setup_transfer_response(USB_SPI_DISABLED);
		} else if (write_count > USB_SPI_MAX_WRITE_COUNT) {
			setup_transfer_response(USB_SPI_WRITE_COUNT_INVALID);
#ifdef CONFIG_SPI_HALFDUPLEX
		} else if (read_count == USB_SPI_FULL_DUPLEX_ENABLED) {
			/* Full duplex mode is not supported on this device. */
			setup_transfer_response(
				USB_SPI_UNSUPPORTED_FULL_DUPLEX);
#endif
		} else if (read_count > USB_SPI_MAX_READ_COUNT &&
			   read_count != USB_SPI_FULL_DUPLEX_ENABLED) {
			setup_transfer_response(USB_SPI_READ_COUNT_INVALID);
		} else {
			usb_spi_setup_transfer(write_count, read_count);
			packet->header_size =
				offsetof(struct usb_spi_command_v2, data);
			usb_spi_state.status_code = usb_spi_read_usb_packet(
				&usb_spi_state.spi_write_ctx, packet);
		}

		/* Send responses if we encountered an error. */
		if (usb_spi_state.status_code != USB_SPI_SUCCESS) {
			setup_transfer_response(usb_spi_state.status_code);
			break;
		}

		/* Start the SPI transfer when we've read all data. */
		if (usb_spi_state.spi_write_ctx.transfer_index ==
		    usb_spi_state.spi_write_ctx.transfer_size) {
			usb_spi_state.mode = USB_SPI_MODE_START_SPI;
		}

		break;
	}
	case USB_SPI_PKT_ID_CMD_TRANSFER_CONTINUE: {
		/*
		 * The host has sent a continue packet for the SPI transfer
		 * which contains additional data payload.
		 */
		packet->header_size =
			offsetof(struct usb_spi_continue_v2, data);
		if (usb_spi_state.status_code == USB_SPI_SUCCESS) {
			usb_spi_state.status_code = usb_spi_read_usb_packet(
				&usb_spi_state.spi_write_ctx, packet);
		}

		/* Send responses if we encountered an error. */
		if (usb_spi_state.status_code != USB_SPI_SUCCESS) {
			setup_transfer_response(usb_spi_state.status_code);
			break;
		}

		/* Start the SPI transfer when we've read all data. */
		if (usb_spi_state.spi_write_ctx.transfer_index ==
		    usb_spi_state.spi_write_ctx.transfer_size) {
			usb_spi_state.mode = USB_SPI_MODE_START_SPI;
		}

		break;
	}
	case USB_SPI_PKT_ID_CMD_CHIP_SELECT: {
		/*
		 * The host is requesting the chip select line be
		 * asserted or deasserted.
		 */
		uint16_t flags = packet->cmd_cs.flags;
		const struct spi_device_t *current_device =
			&spi_devices[usb_spi_state.current_spi_device_idx];

		if (flags & USB_SPI_CHIP_SELECT) {
			/* Set chip select low (asserted). */
			gpio_set_level(current_device->gpio_cs, 0);
		} else {
			/* Set chip select high (adesserted). */
			gpio_set_level(current_device->gpio_cs, 1);
		}
		usb_spi_state.mode = USB_SPI_MODE_SEND_CHIP_SELECT_RESPONSE;
		break;
	}
	default: {
		/* An unknown USB packet was delivered. */
		setup_transfer_response(USB_SPI_RX_UNEXPECTED_PACKET);
		break;
	}
	}
}

/* Deferred function to handle state changes, process USB SPI packets,
 * and construct responses.
 *
 * @param config        USB SPI config
 */
void usb_spi_deferred(void)
{
	int enabled;
	struct usb_spi_packet_ctx *receive_packet =
		&usb_spi_state.receive_packet;
	struct usb_spi_packet_ctx *transmit_packet =
		&usb_spi_state.transmit_packet;
	transmit_packet->packet_size = 0;

	if (IS_ENABLED(CONFIG_USB_SPI_IGNORE_HOST_SIDE_ENABLE))
		enabled = usb_spi_state.enabled_device;
	else
		enabled = usb_spi_state.enabled_device &&
			  usb_spi_state.enabled_host;

	/*
	 * If our overall enabled state has changed we call the board specific
	 * enable or disable routines and save our new state.
	 */
	if (enabled != usb_spi_state.enabled) {
		if (enabled)
			usb_spi_board_enable();
		else
			usb_spi_board_disable();

		usb_spi_state.enabled = enabled;
	}

	/* Read any packets from the endpoint. */

	usb_spi_read_packet(receive_packet);
	if (receive_packet->packet_size) {
		usb_spi_process_rx_packet(receive_packet);
	}

	/* Need to send the USB SPI configuration */
	if (usb_spi_state.mode == USB_SPI_MODE_SEND_CONFIGURATION) {
		create_spi_config_response(transmit_packet);
		usb_spi_write_packet(transmit_packet);
		usb_spi_state.mode = USB_SPI_MODE_IDLE;
		return;
	}
	/* Need to send response to USB SPI chip select. */
	if (usb_spi_state.mode == USB_SPI_MODE_SEND_CHIP_SELECT_RESPONSE) {
		create_spi_chip_select_response(transmit_packet);
		usb_spi_write_packet(transmit_packet);
		usb_spi_state.mode = USB_SPI_MODE_IDLE;
		return;
	}

	/* Start a new SPI transfer. */
	if (usb_spi_state.mode == USB_SPI_MODE_START_SPI) {
		const struct spi_device_t *current_device =
			&spi_devices[usb_spi_state.current_spi_device_idx];
		bool custom_board_driver = current_device->usb_flags &
					   USB_SPI_CUSTOM_SPI_DEVICE;
		uint16_t status_code;
		int read_count = usb_spi_state.spi_read_ctx.transfer_size;
#ifndef CONFIG_SPI_HALFDUPLEX
		/*
		 * Handle the full duplex mode on supported platforms.
		 * The read count is equal to the write count.
		 */
		if (read_count == USB_SPI_FULL_DUPLEX_ENABLED) {
			usb_spi_state.spi_read_ctx.transfer_size =
				usb_spi_state.spi_write_ctx.transfer_size;
			read_count = SPI_READBACK_ALL;
		}
#endif

		if (custom_board_driver) {
			status_code = usb_spi_board_transaction(
				current_device, 0 /* flash_flags */,
				usb_spi_state.spi_write_ctx.buffer,
				usb_spi_state.spi_write_ctx.transfer_size,
				usb_spi_state.spi_read_ctx.buffer, read_count);
		} else {
			status_code = spi_transaction(
				current_device,
				usb_spi_state.spi_write_ctx.buffer,
				usb_spi_state.spi_write_ctx.transfer_size,
				usb_spi_state.spi_read_ctx.buffer, read_count);
		}

		/* Cast the EC status code to USB SPI and start the response. */
		status_code = usb_spi_map_error(status_code);
		setup_transfer_response(status_code);
	}

	if (usb_spi_response_in_progress() && usb_spi_transmitted_packet()) {
		usb_spi_create_spi_transfer_response(transmit_packet);
		usb_spi_write_packet(transmit_packet);
	}
}

/*
 * Sets which SPI modes will be enabled
 *
 * @param config        USB SPI config
 * @param enabled       usb_spi_request indicating which SPI mode is enabled.
 */
void usb_spi_enable(int enabled)
{
	usb_spi_state.enabled_device = enabled;

	hook_call_deferred(usb_spi.deferred, 0);
}

/*
 * STM32 Platform: Receive the data from the endpoint into the packet and
 *  mark the endpoint as ready to accept more data.
 *
 * @param config        USB SPI config
 * @param packet        Destination packet used to store the endpoint data.
 */
static void usb_spi_read_packet(struct usb_spi_packet_ctx *packet)
{
	size_t packet_size;

	if (!usb_spi_received_packet()) {
		/* No data is present on the endpoint. */
		packet->packet_size = 0;
		return;
	}

	/* Copy bytes from endpoint memory. */
	packet_size = btable_ep[usb_spi.endpoint].rx_count & RX_COUNT_MASK;
	memcpy_from_usbram(packet->bytes,
			   (void *)usb_sram_addr(usb_spi.ep_rx_ram),
			   packet_size);
	packet->packet_size = packet_size;
	/* Set endpoint as valid for accepting new packet. */
	STM32_TOGGLE_EP(usb_spi.endpoint, EP_RX_MASK, EP_RX_VALID, 0);
}

/*
 * STM32 Platform: Transmit data from the packet to the endpoint buffer.
 *  If a packet is written, the endpoint will be marked valid for transmitting.
 *
 * @param config        USB SPI config
 * @param packet        Source packet we will write to the endpoint data.
 */
static void usb_spi_write_packet(struct usb_spi_packet_ctx *packet)
{
	if (packet->packet_size == 0)
		return;

	/* Copy bytes to endpoint memory. */
	memcpy_to_usbram((void *)usb_sram_addr(usb_spi.ep_tx_ram),
			 packet->bytes, packet->packet_size);
	btable_ep[usb_spi.endpoint].tx_count = packet->packet_size;

	/* Mark the packet as having no data. */
	packet->packet_size = 0;

	/* Set endpoint as valid for transmitting new packet. */
	STM32_TOGGLE_EP(usb_spi.endpoint, EP_TX_MASK, EP_TX_VALID, 0);
}

/*
 * STM32 Platform: Returns the RX endpoint status
 *
 * @param config        USB SPI config
 *
 * @returns             Returns true when the RX endpoint has a packet.
 */
static bool usb_spi_received_packet(void)
{
	return (STM32_USB_EP(usb_spi.endpoint) & EP_RX_MASK) != EP_RX_VALID;
}

/* STM32 Platform: Returns the TX endpoint status
 *
 * @param config        USB SPI config
 *
 * @returns             Returns true when the TX endpoint transmitted
 *                      the packet written.
 */
static bool usb_spi_transmitted_packet(void)
{
	return (STM32_USB_EP(usb_spi.endpoint) & EP_TX_MASK) != EP_TX_VALID;
}

/* STM32 Platform: Handle interrupt for USB data received.
 *
 * @param config        USB SPI config
 */
void usb_spi_rx(void)
{
	/*
	 * We need to set both the TX and RX endpoints to NAK to prevent
	 * transfers. The protocol requires responses to follow a command, but
	 * the USB host will request the next packet from the TX endpoint
	 * before the USB SPI has updated the memory in the buffer. By setting
	 * it to NAK in the ISR, it will not perform a transfer until the
	 * next packet is ready.
	 *
	 * This has a side effect of disabling the endpoint interrupts until
	 * they are set to valid or a USB reset events occurs.
	 */
	STM32_TOGGLE_EP(usb_spi.endpoint, EP_TX_RX_MASK, EP_TX_RX_NAK, 0);

	hook_call_deferred(usb_spi.deferred, 0);
}

/*
 * STM32 Platform: Handle interrupt for USB data transmitted.
 *
 * @param config        USB SPI config
 */
void usb_spi_tx(void)
{
	STM32_TOGGLE_EP(usb_spi.endpoint, EP_TX_MASK, EP_TX_NAK, 0);

	hook_call_deferred(usb_spi.deferred, 0);
}

/*
 * STM32 Platform: Handle interrupt for USB events
 *
 * @param config        USB SPI config
 * @param evt           USB event
 */
void usb_spi_event(enum usb_ep_event evt)
{
	int endpoint;

	if (evt != USB_EVENT_RESET)
		return;

	endpoint = usb_spi.endpoint;

	usb_spi_reset_interface();

	btable_ep[endpoint].tx_addr = usb_sram_addr(usb_spi.ep_tx_ram);
	btable_ep[endpoint].tx_count = 0;

	btable_ep[endpoint].rx_addr = usb_sram_addr(usb_spi.ep_rx_ram);
	btable_ep[endpoint].rx_count = 0x8000 |
				       ((USB_MAX_PACKET_SIZE / 32 - 1) << 10);

	STM32_USB_EP(endpoint) = ((endpoint << 0) | /* Endpoint Addr*/
				  (2 << 4) | /* TX NAK */
				  (0 << 9) | /* Bulk EP */
				  (3 << 12)); /* RX Valid */
}

/*
 * STM32 Platform: Handle control transfers.
 *
 * @param config        USB SPI config
 * @param rx_buf        Contains setup packet
 * @param tx_buf        unused
 */
int usb_spi_interface(usb_uint *rx_buf, usb_uint *tx_buf)
{
	struct usb_setup_packet setup;

	usb_read_setup_packet(rx_buf, &setup);

	if (setup.bmRequestType !=
	    (USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE))
		return 1;

	if (setup.wValue >= spi_devices_used ||
	    !(spi_devices[setup.wValue].usb_flags & USB_SPI_ENABLED) ||
	    setup.wIndex != usb_spi.interface || setup.wLength != 0)
		return 1;

	/* Record which SPI device the host wished to manipulate. */
	usb_spi_state.current_spi_device_idx = setup.wValue;

	switch (setup.bRequest) {
	case USB_SPI_REQ_ENABLE:
		usb_spi_state.enabled_host = 1;
		break;

	case USB_SPI_REQ_DISABLE:
		usb_spi_state.enabled_host = 0;
		break;

	default:
		return 1;
	}

	/*
	 * Our state has changed, call the deferred function to handle the
	 * state change.
	 */
	if (!IS_ENABLED(CONFIG_USB_SPI_IGNORE_HOST_SIDE_ENABLE))
		hook_call_deferred(usb_spi.deferred, 0);

	usb_spi_reset_interface();

	btable_ep[0].tx_count = 0;
	STM32_TOGGLE_EP(0, EP_TX_RX_MASK, EP_TX_RX_VALID, EP_STATUS_OUT);
	return 0;
}

__overridable int
usb_spi_board_transaction(const struct spi_device_t *spi_device,
			  uint32_t flash_flags, const uint8_t *txdata,
			  int txlen, uint8_t *rxdata, int rxlen)
{
	return EC_ERROR_UNIMPLEMENTED;
}
