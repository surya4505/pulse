/***
  This file is part of PulseAudio.

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "core-format.h"

#include <pulse/def.h>
#include <pulse/xmalloc.h>

#include <pulsecore/macro.h>

int pa_format_info_get_sample_format(pa_format_info *f, pa_sample_format_t *sf) {
    int r;
    char *sf_str;
    pa_sample_format_t sf_local;

    pa_assert(f);
    pa_assert(sf);

    r = pa_format_info_get_prop_string(f, PA_PROP_FORMAT_SAMPLE_FORMAT, &sf_str);
    if (r < 0)
        return r;

    sf_local = pa_parse_sample_format(sf_str);
    pa_xfree(sf_str);

    if (!pa_sample_format_valid(sf_local)) {
        pa_log_debug("Invalid sample format.");
        return -PA_ERR_INVALID;
    }

    *sf = sf_local;

    return 0;
}

int pa_format_info_get_rate(pa_format_info *f, uint32_t *rate) {
    int r;
    int rate_local;

    pa_assert(f);
    pa_assert(rate);

    r = pa_format_info_get_prop_int(f, PA_PROP_FORMAT_RATE, &rate_local);
    if (r < 0)
        return r;

    if (!pa_sample_rate_valid(rate_local)) {
        pa_log_debug("Invalid sample rate: %i", rate_local);
        return -PA_ERR_INVALID;
    }

    *rate = rate_local;

    return 0;
}

int pa_format_info_get_channels(pa_format_info *f, uint8_t *channels) {
    int r;
    int channels_local;

    pa_assert(f);
    pa_assert(channels);

    r = pa_format_info_get_prop_int(f, PA_PROP_FORMAT_CHANNELS, &channels_local);
    if (r < 0)
        return r;

    if (!pa_channels_valid(channels_local)) {
        pa_log_debug("Invalid channel count: %i", channels_local);
        return -PA_ERR_INVALID;
    }

    *channels = channels_local;

    return 0;
}

int pa_format_info_to_sample_spec_fake(pa_format_info *f, pa_sample_spec *ss, pa_channel_map *map) {
    int rate;

    pa_assert(f);
    pa_assert(ss);

    /* Note: When we add support for non-IEC61937 encapsulated compressed
     * formats, this function should return a non-zero values for these. */

    ss->format = PA_SAMPLE_S16LE;
    ss->channels = 2;

    if (map)
        pa_channel_map_init_stereo(map);

    pa_return_val_if_fail(pa_format_info_get_prop_int(f, PA_PROP_FORMAT_RATE, &rate) == 0, -PA_ERR_INVALID);
    ss->rate = (uint32_t) rate;

    if (f->encoding == PA_ENCODING_EAC3_IEC61937)
        ss->rate *= 4;

    return 0;
}
