
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "audio.h"
#include "pci_hda.h"

static int audio_debug = 0;
#define   DPRINTF(params) if (audio_debug) printf params

/*
 * Audio Player internal data structures
 */

struct audio {
	int fd;
	uint8_t dir;
	char dev_name[64];
};

/*
 * Audio Player module function definitions
 */

/*
 * audio_init - initialize an instance of audio player
 * @dev_name - the backend sound device used to play / capture
 * @dir - dir = 1 for write mode, dir = 0 for read mode
 */
struct audio *audio_init(const char *dev_name, uint8_t dir)
{
	struct audio *aud = NULL;

	assert(dev_name);

	aud = calloc(1, sizeof(*aud));
	if (!aud)
		return NULL;

	if (strlen(dev_name) < sizeof(aud->dev_name))
		memcpy(aud->dev_name, dev_name, strlen(dev_name) + 1);
	else {
		DPRINTF(("dev_name too big\n"));
		free(aud);
		return NULL;
	}

	aud->fd = -1;
	aud->dir = dir;

	return aud;
}

/*
 * audio_set_params - reset the sound device and set the audio params
 * @aud - the audio player to be configured
 * @params - the audio parameters to be set
 */
int audio_set_params(struct audio *aud, struct audio_params *params)
{
	int audio_fd = -1;
	int format, channels, rate;
	int err;
#if DEBUG_HDA == 1
	audio_buf_info info;
#endif

	assert(aud);
	assert(params);

	/* Close the device if was opened before */
	if (aud->fd != -1) {
		err = close(aud->fd);
		if (err == -1) {
			DPRINTF(("Fail to close fd: %d, errno: %d\n", aud->fd, errno));
			return -1;
		}
	}

	audio_fd = open(aud->dev_name, aud->dir ? O_WRONLY : O_RDONLY, 0);
	if (audio_fd == -1) {
		DPRINTF(("Fail to open dev: %s, errno: %d\n", aud->dev_name, errno));
		return -1;
	}

	aud->fd = audio_fd;

	/* Set the Format (Bits per Sample) */
	format = params->format;
	err = ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format);
	if (err == -1) {
		DPRINTF(("Fail to set fmt: 0x%x errno: %d\n", params->format, errno));
		return -1;
	}

	/* The device does not support the requested audio format */
	if (format != params->format) {
		DPRINTF(("Mismatch format: 0x%x params->format: 0x%x\n", format, params->format));
		return -1;
	}

	/* Set the Number of Channels */
	channels = params->channels;
	err = ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &channels);
	if (err == -1) {
		DPRINTF(("Fail to set channels: %d errno: %d\n", params->channels, errno));
		return -1;
	}

	/* The device does not support the requested no. of channels */
	if (channels != params->channels) {
		DPRINTF(("Mismatch channels: %d params->channels: %d\n", channels, params->channels));
		return -1;
	}

	/* Set the Sample Rate / Speed */
	rate = params->rate;
	err = ioctl(audio_fd, SNDCTL_DSP_SPEED, &rate);
	if (err == -1) {
		DPRINTF(("Fail to set speed: %d errno: %d\n", params->rate, errno));
		return -1;
	}

	/* The device does not support the requested rate / speed */
	if (rate != params->rate) {
		DPRINTF(("Mismatch rate: %d params->rate: %d\n", rate, params->rate));
		return -1;
	}

#if DEBUG_HDA == 1
	err = ioctl(audio_fd, aud->dir ? SNDCTL_DSP_GETOSPACE : SNDCTL_DSP_GETISPACE, &info);
	if (err == -1) {
		DPRINTF(("Fail to get audio buf info errno: %d\n", errno));
		return -1;
	}
	DPRINTF(("fragstotal: 0x%x fragsize: 0x%x\n", info.fragstotal, info.fragsize));
#endif
	return 0;
}

/*
 * audio_playback - plays samples to the sound device using blocking operations
 * @aud - the audio player used to play the samples
 * @buf - the buffer containing the samples
 * @count - the number of bytes in buffer
 */
int audio_playback(struct audio *aud, const void *buf, size_t count)
{
	int audio_fd = -1;
	ssize_t len = 0, total = 0;

	assert(aud);
	assert(aud->dir);
	assert(buf);

	audio_fd = aud->fd;
	assert(audio_fd != -1);

	total = 0;
	while (total < count) {
		len = write(audio_fd, buf + total, count - total);
		if (len == -1) {
			DPRINTF(("Fail to write to fd: %d, errno: %d\n", audio_fd, errno));
			return -1;
		}

		total += len;
	}

	return 0;
}

/*
 * audio_record - records samples from the sound device using blocking operations
 * @aud - the audio player used to capture the samples
 * @buf - the buffer to receive the samples
 * @count - the number of bytes to capture in buffer
 * Returns -1 on error and 0 on success
 */
int audio_record(struct audio *aud, void *buf, size_t count)
{
	int audio_fd = -1;
	ssize_t len = 0, total = 0;

	assert(aud);
	assert(!aud->dir);
	assert(buf);

	audio_fd = aud->fd;
	assert(audio_fd != -1);

	total = 0;
	while (total < count) {
		len = read(audio_fd, buf + total, count - total);
		if (len == -1) {
			DPRINTF(("Fail to write to fd: %d, errno: %d\n", audio_fd, errno));
			return -1;
		}

		total += len;
	}

	return 0;
}

