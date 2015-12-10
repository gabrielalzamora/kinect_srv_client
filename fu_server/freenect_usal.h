/*****************************************************************************

File        : freenect_usal.h

Proyect     : API Kinect
Copyright (c) 2015  Gabriel Alzamora

based on OpenKinect examples
Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file

Author      : Gabriel Alzamora     <gabrielalzamora@usal.es>
Tutor       : Raul Alves           <ralves@usal.es>
Version     : 1.0
Description : read Kinect & show on QT gui

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#ifndef FREENECT_USAL
#define FREENECT_USAL

#include <sys/types.h>
#include <pthread.h>
#include "libfreenect.h"

#define W 640
#define H 480

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t fu_thread;
int die = 0;// flag to exit fu_thread_function
int got_rgb =0;// flag
u_int32_t timestamp;
u_int8_t *rgb_back, *rgb_mid, *rgb_front;//buffers

// freenect
freenect_device *device;
freenect_context *context;
int led = LED_OFF;
int ang = 0;
freenect_video_format current_format = FREENECT_VIDEO_RGB;
freenect_video_format requested_format = FREENECT_VIDEO_RGB;

void fu_video_callback(freenect_device *dev, void *rgb, u_int32_t timestamp)
{
    pthread_mutex_lock(&buffer_mutex);// mutex no one else may touch buffers
    // swap buffers
    rgb_back = rgb_mid;// rgb_mid points to old loaded data so do back
    freenect_set_video_buffer(dev, rgb_back);//assign rgb_back to load new data
    rgb_mid = (uint8_t*)rgb;//buffers swapped mid point to last data and back will load new data
    got_rgb++;//flag says has data
    //pthread_cond_signal(&gl_frame_cond);// conditional mutex->DRAWGLScene()<-glview.c
    pthread_mutex_unlock(&buffer_mutex);// release mutex
}

void *fu_thread_function(void *arg)
{
    freenect_set_tilt_degs(device,ang);
    freenect_set_led(device,LED_GREEN);
    //freenect_set_depth_callback(device, fu_depth_cb);
    freenect_set_video_callback(device, fu_video_callback);
    freenect_set_video_mode(device, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, current_format));
    //freenect_set_depth_mode(device, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
    freenect_set_video_buffer(device, rgb_back);// back as video buffer to load new data
    //freenect_start_depth(device);
    freenect_start_video(device);

    while (!die && freenect_process_events(context) >= 0) {// die flag will finish this
        if (requested_format != current_format) {// change video_format in runtime
            freenect_stop_video(device);
            freenect_set_video_mode(device, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, requested_format));
            freenect_start_video(device);
            current_format = requested_format;
        }
    }

    //printf("\nshutting down streams...\n");
    //freenect_stop_depth(device);
    freenect_stop_video(device);
    freenect_close_device(device);
    freenect_shutdown(context);
    //printf("-- done!\n");
    return NULL;
}

#endif // FREENECT_USAL

