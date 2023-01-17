#ifndef MAIN_H
#define MAIN_H

// dynamic
// # define htow(h) ((term_w*1080)/(term_h*1920) *(h))

// static
#define htow(h) (2*h)


#define window_h 40
#define window_w htow(window_h)

#endif
