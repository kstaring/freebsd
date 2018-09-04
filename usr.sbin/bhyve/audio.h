
#ifndef _AUDIO_EMUL_H_ 
#define _AUDIO_EMUL_H_

#include <sys/types.h>
#include <sys/soundcard.h>

/*
 * Audio Player data structures
 */

struct audio;

struct audio_params {
	int format;
	int channels;
	int rate;
};

/*
 * Audio Player API
 */

/*
 * audio_init - initialize an instance of audio player
 * @dev_name - the backend sound device used to play / capture
 * @dir - dir = 1 for write mode, dir = 0 for read mode
 * Returns NULL on error and the address of the audio player instance
 */
struct audio *audio_init(const char *dev_name, uint8_t dir);

/*
 * audio_set_params - reset the sound device and set the audio params
 * @aud - the audio player to be configured
 * @params - the audio parameters to be set
 * Returns -1 on error and 0 on success
 */
int audio_set_params(struct audio *aud, struct audio_params *params);

/*
 * audio_playback - plays samples to the sound device using blocking operations
 * @aud - the audio player used to play the samples
 * @buf - the buffer containing the samples
 * @count - the number of bytes in buffer
 * Returns -1 on error and 0 on success
 */
int audio_playback(struct audio *aud, const void *buf, size_t count);

/*
 * audio_record - records samples from the sound device using blocking operations
 * @aud - the audio player used to capture the samples
 * @buf - the buffer to receive the samples
 * @count - the number of bytes to capture in buffer
 * Returns -1 on error and 0 on success
 */
int audio_record(struct audio *aud, void *buf, size_t count);

#endif  /* _AUDIO_EMUL_H_ */
