/*
 * YangTsTime.cpp
 *
 *  Created on: 2020年9月6日
 *      Author: yang
 */

#include "YangTsTime.h"

YangTsTime::YangTsTime() {
	// TODO Auto-generated constructor stub

}

YangTsTime::~YangTsTime() {
	// TODO Auto-generated destructor stub
}

void YangTsTime::test(){

}


/**
static void video_refresh(void *opaque, double *remaining_time) {
	VideoState *is = (VideoState*)opaque;
	double time;

	if (is->video_st) {
	retry:
		if (frame_queue_nb_remaining(&is->pictq) == 0) {
			// nothing to do, no picture to display in the queue
		}
		else {
			double last_duration, duration, delay;
			Frame *vp, *lastvp;

			// dequeue the picture
			lastvp = frame_queue_peek_last(&is->pictq); // f->rindex
			vp = frame_queue_peek(&is->pictq); // f->rindex + f->rindex_shown

			if (vp->serial != is->videoq.serial) {
				frame_queue_next(&is->pictq);
				goto retry;
			}

			if (lastvp->serial != vp->serial)
				is->frame_timer = av_gettime_relative() / 1000000.0;

			if (is->paused)
				goto display;

			//compute nominal last_duration
			last_duration = vp_duration(is, lastvp, vp);
			delay = compute_target_delay(last_duration, is);

			time = av_gettime_relative() / 1000000.0;
			if (time < is->frame_timer + delay) {
				*remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
				goto display;
			}

			is->frame_timer += delay;
			if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
				is->frame_timer = time;

			SDL_LockMutex(is->pictq.mutex);
			if (!isnan(vp->pts))
				update_video_pts(is, vp->pts, vp->serial);
			SDL_UnlockMutex(is->pictq.mutex);

			// 丢帧？？
			if (frame_queue_nb_remaining(&is->pictq) > 1) {
				Frame *nextvp = frame_queue_peek_next(&is->pictq); // f->rindex + f->rindex_shown + 1
				duration = vp_duration(is, vp, nextvp);
				if (time > is->frame_timer + duration) {
					frame_queue_next(&is->pictq);
					goto retry;
				}
			}

			frame_queue_next(&is->pictq);
			is->force_refresh = 1;
		}
	display:
		//display picture
		if (is->force_refresh && is->pictq.rindex_shown)
			video_display(is);
	}
	is->force_refresh = 0;
}

**/
