/*

 MIT License
 
 Copyright © 2020 Samuel Venable
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
*/

#include "VideoPlayer/videoplayer.h"

using std::string;

#ifdef _WIN32
#define EXPORTED_FUNCTION extern "C" __declspec(dllexport)
#else /* Linux, macOS, and BSD */
#define EXPORTED_FUNCTION extern "C" __attribute__((visibility("default"))) 
#endif

EXPORTED_FUNCTION double splash_set_stop_mouse(double stop) {
  enigma_user::splash_set_stop_mouse((bool)stop);
  return 0;
}

EXPORTED_FUNCTION double splash_set_stop_key(double stop) {
  enigma_user::splash_set_stop_key((bool)stop);
  return 0;
}

EXPORTED_FUNCTION double splash_set_window(char *wid) {
  enigma_user::splash_set_window(wid);
  return 0;
}

EXPORTED_FUNCTION double splash_set_volume(double vol) {
  enigma_user::splash_set_volume((int)vol);
  return 0;
}

EXPORTED_FUNCTION double splash_show_video(char *fname, double loop) {
  enigma_user::splash_show_video(fname, (bool)loop);
  return 0;
}

EXPORTED_FUNCTION char *video_add(char *fname) {
  static string result;
  result = enigma_user::video_add(fname);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double video_get_option_was_set(char *ind, char *option) {
  return enigma_user::video_get_option_was_set(ind, option);
}

EXPORTED_FUNCTION char *video_get_option_string(char *ind, char *option) {
  static string result;
  result = enigma_user::video_get_option_string(ind, option);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double video_set_option_string(char *ind, char *option, char *value) {
  enigma_user::video_set_option_string(ind, option, value);
  return 0;
}

EXPORTED_FUNCTION double video_play(char *ind) {
  enigma_user::video_play(ind);
  return 0;
}

EXPORTED_FUNCTION double video_is_paused(char *ind) {
  return enigma_user::video_is_paused(ind);
}

EXPORTED_FUNCTION double video_is_playing(char *ind) {
  return enigma_user::video_is_playing(ind);
}

EXPORTED_FUNCTION double video_get_fullscreen(char *ind) {
  return enigma_user::video_get_fullscreen(ind);
}

EXPORTED_FUNCTION double video_set_fullscreen(char *ind, double fullscreen) {
  enigma_user::video_set_fullscreen(ind, (bool)fullscreen);
  return 0;
}

EXPORTED_FUNCTION double video_get_volume_percent(char *ind) {
  return enigma_user::video_get_volume_percent(ind);
}

EXPORTED_FUNCTION double video_set_volume_percent(char *ind, double volume) {
  enigma_user::video_set_volume_percent(ind, (int)volume);
  return 0;
}

EXPORTED_FUNCTION char *video_get_window_identifier(char *ind) {
  static string result;
  result = enigma_user::video_get_window_identifier(ind);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double video_set_window_identifier(char *ind, char *wid) {
  enigma_user::video_set_window_identifier(ind, wid);
  return 0;
}

EXPORTED_FUNCTION double video_exists(char *ind) {
  return enigma_user::video_exists(ind);
}

EXPORTED_FUNCTION double video_pause(char *ind) {
  enigma_user::video_pause(ind);
  return 0;
}

EXPORTED_FUNCTION double video_stop(char *ind) {
  enigma_user::video_stop(ind);
  return 0;
}

EXPORTED_FUNCTION double video_get_width(char *ind) {
  return enigma_user::video_get_width(ind);
}

EXPORTED_FUNCTION double video_get_height(char *ind) {
  return enigma_user::video_get_height(ind);
}

EXPORTED_FUNCTION double video_get_duration(char *ind) {
  return enigma_user::video_get_duration(ind);
}

EXPORTED_FUNCTION double video_delete(char *ind) {
  enigma_user::video_delete(ind);
  return 0;
}
