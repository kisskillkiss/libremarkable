#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h> 
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include "libremarkable/lib.h"

int get_random(int min, int max) {
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void draw_rect(remarkable_framebuffer* fb, mxcfb_rect rect, remarkable_color color) {
  if (fb == NULL)
    return;

  int offset = 0;
  for (unsigned y = rect.top; y < rect.height + rect.top; ++y) {
    if (y >= fb->vinfo.yres_virtual)
      break;
    for (unsigned x = rect.left; x < rect.width + rect.left; ++x) {
      if (x >= fb->vinfo.xres_virtual)
        break;
      offset = y * fb->finfo.line_length + x;
      if (offset >= fb->finfo.smem_len)
        break;
      *(fb->mapped_buffer + offset) = color;
    }
  }
}

int main(void) {
  remarkable_framebuffer* fb = remarkable_framebuffer_init("/dev/fb0");
  if (fb == NULL) {
    printf("remarkable_framebuffer_init('/dev/fb0') returned NULL. Exiting.\n");
    exit(1);
  }

  // Clear the screen and refresh
  remarkable_framebuffer_fill(fb, REMARKABLE_BRIGHTEST);
  remarkable_framebuffer_refresh(fb);

  srand(time(NULL));

  // Draw a rectangle and only update that region
  mxcfb_rect rect;

  for (unsigned i = 0; i < 10; i++) {
    rect.top = get_random(0, 500);
    rect.left = get_random(0, 500);
    rect.height = 100;
    rect.width = 100;
    draw_rect(fb, rect, REMARKABLE_DARKEST);
    remarkable_framebuffer_partial_refresh(fb, rect);

    sleep(1);
  }


  remarkable_framebuffer_destroy(fb);
  return 0;
}