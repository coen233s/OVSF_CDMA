#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG
#define dout(x...) cout << x
#else
#define dout(...)
#endif

#ifdef VERBOSE
#define vout(x...) cout << x
#else
#define vout(...)
#endif

#endif /* DEBUG_H_ */
