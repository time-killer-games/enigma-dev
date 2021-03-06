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

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "../procinfo.h"
#include "subclass.h"

#include <Cocoa/Cocoa.h>
#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::vector;
using std::to_string;

NSWindow *cocoa_window_from_wid(CGWindowID wid) {
  return [NSApp windowWithWindowNumber:wid];
}

CGWindowID cocoa_wid_from_window(NSWindow *window) {
  return [window windowNumber];
}

bool cocoa_wid_exists(CGWindowID wid) {
  bool result = false;
  const CGWindowLevel kScreensaverWindowLevel = CGWindowLevelForKey(kCGScreenSaverWindowLevelKey);
  CFArrayRef windowArray = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
  CFIndex windowCount = 0;
  if ((windowCount = CFArrayGetCount(windowArray))) {
    for (CFIndex i = 0; i < windowCount; i++) {
      NSDictionary *windowInfoDictionary =
      (__bridge NSDictionary *)((CFDictionaryRef)CFArrayGetValueAtIndex(windowArray, i));
      NSNumber *ownerPID = (NSNumber *)(windowInfoDictionary[(id)kCGWindowOwnerPID]);
      NSNumber *level = (NSNumber *)(windowInfoDictionary[(id)kCGWindowLayer]);
      if (level.integerValue < kScreensaverWindowLevel) {
        NSNumber *windowID = windowInfoDictionary[(id)kCGWindowNumber];
        if (wid == windowID.integerValue) {
          result = true;
          break;
        }
      }
    }
  }
  CFRelease(windowArray);
  return result;
}

pid_t cocoa_pid_from_wid(CGWindowID wid) {
  pid_t pid;
  const CGWindowLevel kScreensaverWindowLevel = CGWindowLevelForKey(kCGScreenSaverWindowLevelKey);
  CFArrayRef windowArray = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
  CFIndex windowCount = 0;
  if ((windowCount = CFArrayGetCount(windowArray))) {
    for (CFIndex i = 0; i < windowCount; i++) {
      NSDictionary *windowInfoDictionary =
      (__bridge NSDictionary *)((CFDictionaryRef)CFArrayGetValueAtIndex(windowArray, i));
      NSNumber *ownerPID = (NSNumber *)(windowInfoDictionary[(id)kCGWindowOwnerPID]);
      NSNumber *level = (NSNumber *)(windowInfoDictionary[(id)kCGWindowLayer]);
      if (level.integerValue < kScreensaverWindowLevel) {
        NSNumber *windowID = windowInfoDictionary[(id)kCGWindowNumber];
        if (wid == windowID.integerValue) {
          pid = ownerPID.integerValue;
          break;
        }
      }
    }
  }
  CFRelease(windowArray);
  return pid;
}

const char *cocoa_wids_from_pid(pid_t pid) {
  NSString *wids = [[NSString alloc] init];
  const CGWindowLevel kScreensaverWindowLevel = CGWindowLevelForKey(kCGScreenSaverWindowLevelKey);
  CFArrayRef windowArray = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
  CFIndex windowCount = 0;
  if ((windowCount = CFArrayGetCount(windowArray))) {
    for (CFIndex i = 0; i < windowCount; i++) {
      NSDictionary *windowInfoDictionary =
      (__bridge NSDictionary *)((CFDictionaryRef)CFArrayGetValueAtIndex(windowArray, i));
      NSNumber *ownerPID = (NSNumber *)(windowInfoDictionary[(id)kCGWindowOwnerPID]);
      NSNumber *level = (NSNumber *)(windowInfoDictionary[(id)kCGWindowLayer]);
      if (level.integerValue < kScreensaverWindowLevel) {
        NSNumber *windowID = windowInfoDictionary[(id)kCGWindowNumber];
        if (pid == ownerPID.integerValue) {
          wids = [wids stringByAppendingString:[@(windowID.integerValue) stringValue]];
          wids = [wids stringByAppendingString:@"|"];
        }
      }
    }
  }
  if ([wids length] > 0) {
    wids = [wids substringWithRange:NSMakeRange(0, [wids length] - 1)];
  }
  const char *result = [wids UTF8String];
  CFRelease(windowArray);
  [wids release];
  return result;
}

unsigned long cocoa_get_wid_or_pid(bool wid) {
  unsigned long result;
  const CGWindowLevel kScreensaverWindowLevel = CGWindowLevelForKey(kCGScreenSaverWindowLevelKey);
  CFArrayRef windowArray = CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
  CFIndex windowCount = 0;
  if ((windowCount = CFArrayGetCount(windowArray))) {
    for (CFIndex i = 0; i < windowCount; i++) {
      NSDictionary *windowInfoDictionary =
      (__bridge NSDictionary *)((CFDictionaryRef)CFArrayGetValueAtIndex(windowArray, i));
      NSNumber *ownerPID = (NSNumber *)(windowInfoDictionary[(id)kCGWindowOwnerPID]);
      NSNumber *level = (NSNumber *)(windowInfoDictionary[(id)kCGWindowLayer]);
      if (level.integerValue == 0) {
        NSNumber *windowID = windowInfoDictionary[(id)kCGWindowNumber];
        result = wid ? windowID.integerValue : ownerPID.integerValue;
        break;
      }
    }
  }
  CFRelease(windowArray);
  return result;
}

void cocoa_wid_to_top(CGWindowID wid) {
  CFIndex appCount = [[[NSWorkspace sharedWorkspace] runningApplications] count];
  for (CFIndex i = 0; i < appCount; i++) {
    NSWorkspace *sharedWS = [NSWorkspace sharedWorkspace];
    NSArray *runningApps = [sharedWS runningApplications];
    NSRunningApplication *currentApp = [runningApps objectAtIndex:i];
    pid_t currentPid = [currentApp processIdentifier];
    if (cocoa_pid_from_wid(wid) == currentPid) {
      NSRunningApplication *appWithPID = currentApp;
      NSUInteger options = NSApplicationActivateAllWindows;
      options |= NSApplicationActivateIgnoringOtherApps;
      [appWithPID activateWithOptions:options];
      break;
    }
  }
}

void cocoa_wid_set_pwid(CGWindowID wid, CGWindowID pwid) {
  if (cocoa_pid_from_wid(pwid) == getpid()) {
    [cocoa_window_from_wid(pwid) setChildWindowWithNumber:wid];
  } else if (cocoa_pid_from_wid(wid) == getpid()) {
    [cocoa_window_from_wid(wid) setParentWindowWithNumber:pwid];
  }
}

namespace procinfo {

bool wid_exists(wid_t wid) {
  return cocoa_wid_exists(stoul(wid, nullptr, 10));
}

window_t window_from_wid(wid_t wid) {
  unsigned long ul_wid = stoul(wid, nullptr, 10);
  void *voidp_window = reinterpret_cast<void *>(cocoa_window_from_wid(ul_wid));
  return reinterpret_cast<unsigned long long>(voidp_window);
}

wid_t wid_from_window(window_t window) {
  unsigned long long ull_window = reinterpret_cast<unsigned long long>(window);
  void *voidp_window = reinterpret_cast<void *>(ull_window);
  return to_string(cocoa_wid_from_window(reinterpret_cast<NSWindow *>(voidp_window)));
}

PROCID pid_from_wid(wid_t wid) {
  return cocoa_pid_from_wid(stoul(wid, nullptr, 10));
}

string wids_from_pid(PROCID pid) {
  return cocoa_wids_from_pid(pid);
}

wid_t wid_from_top() {
  return to_string(cocoa_get_wid_or_pid(true));
}

PROCID pid_from_top() {
  return cocoa_get_wid_or_pid(false);
}

void wid_to_top(wid_t wid) {
  cocoa_wid_to_top(stoul(wid, nullptr, 10));
}

void wid_set_pwid(wid_t wid, wid_t pwid) {
  cocoa_wid_set_pwid(stoul(wid, nullptr, 10), stoul(pwid, nullptr, 10));
}

} // namespace procinfo
